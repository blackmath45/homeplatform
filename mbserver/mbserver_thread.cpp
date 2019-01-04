#include "mbserver_thread.h"

void mbserver_thread::SetConnectionString(string sConnectionString) { this->sConnectionString = sConnectionString; }
void mbserver_thread::SetIP(string sIP) { this->sIP = sIP; }
void mbserver_thread::SetQueueAdamReadToPLC(cqueue<adamtag> *cqAdamReadToPLC) { this->cqAdamReadToPLC = cqAdamReadToPLC; }
void mbserver_thread::SetQueueAdamResponseFromPLC(cqueue<adamtag> *cqAdamResponseFromPLC) { this->cqAdamResponseFromPLC = cqAdamResponseFromPLC; }
void mbserver_thread::SetQueueAdamWriteToPLC(cqueue<adamtag> *cqAdamWriteToPLC) { this->cqAdamWriteToPLC = cqAdamWriteToPLC; }
void mbserver_thread::SetQueueReadChange(cqueue<datatag> *cqReadChange) { this->cqReadChange = cqReadChange; }
void mbserver_thread::SetQueueWriteRequest(cqueue<datatag> *cqWriteRequest) { this->cqWriteRequest = cqWriteRequest; }
	
void mbserver_thread::ThreadStart()
{
	thargs.cConnectionString = strdup(this->sConnectionString.c_str());
	thargs.cIP = strdup(this->sIP.c_str());

	thargs.cqAdamReadToPLC = this->cqAdamReadToPLC;
	thargs.cqAdamResponseFromPLC = this->cqAdamResponseFromPLC;	
	thargs.cqAdamWriteToPLC = this->cqAdamWriteToPLC;

	thargs.cqReadChange = this->cqReadChange;	
	thargs.cqWriteRequest = this->cqWriteRequest;

	std::clog << kLogNotice << "mbserver_thread : starting thread" << std::endl;
	pthread_create(&this->thread_id, NULL, &mbserver_thread::ThreadWrapper, (void *)&thargs);
}

void mbserver_thread::ThreadJoin()
{
	pthread_join(this->thread_id, NULL);
	std::clog << kLogNotice << "mbserver_thread : thread join" << std::endl;
}

void mbserver_thread::threadFunction(void* args)
{
	std::clog << kLogNotice << "mbserver_thread : thread started" << std::endl;
	
	//*************** ARGS ***************
	struct threadArgsStruct *thargs = (struct threadArgsStruct*)args;
	char *cConnectionString;
	char *cIP;
	cConnectionString = (char *)thargs->cConnectionString;
	cIP = (char *)thargs->cIP;
	string sConnectionString(cConnectionString);	
	string sIP(cIP);	
	
	cqueue<adamtag> *cqAdamReadToPLC = (cqueue<adamtag> *)thargs->cqAdamReadToPLC;
	cqueue<adamtag> *cqAdamResponseFromPLC = (cqueue<adamtag> *)thargs->cqAdamResponseFromPLC;
	cqueue<adamtag> *cqAdamWriteToPLC = (cqueue<adamtag> *)thargs->cqAdamWriteToPLC;

	cqueue<datatag> *cqReadChange = (cqueue<datatag> *)thargs->cqReadChange;
	cqueue<datatag> *cqWriteRequest = (cqueue<datatag> *)thargs->cqWriteRequest;	
	//************************************
		
	modbus_t *ctx;
	int ctxerror = 0;
	int rc_read;
	int rc_write;	
	int flagSendMessage;
	int MBLength;
	uint16_t tab_reg[64];
	int16_t tmpsigned;
	string sValue;
	
	mbserver_conf *conf = new mbserver_conf();
	
	conf->SetConnectionString(sConnectionString);
	conf->LoadConf();
	
	if (conf->GetList()->empty())
	{
		std::clog << kLogWarning << "mbserver_thread : Empty list" << std::endl;
		return;
	}

	std::clog << kLogNotice << "mbserver_thread : modbus_new_tcp" << std::endl;
	ctx = modbus_new_tcp (sIP.c_str(), 502);
	if (ctx == NULL) 
	{
		std::clog << kLogWarning << "mbserver_thread : Unable to create the libmodbus context" << std::endl;
		ctxerror = -1;
	}
	
	while(1)
	{
		ctxerror = 0;
		
		sleep(1);
				
		std::clog << kLogNotice << "mbserver_thread : modbus_connect" << std::endl;
		if (modbus_connect(ctx) == -1) 
		{
			std::clog << kLogWarning << "mbserver_thread : Connection failed " << modbus_strerror(errno) << std::endl;
			//fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
			ctxerror = -1;
			modbus_close(ctx);
			continue;
		}
		
		while (ctxerror >= 0) 
		{
			//time_t t;
			

			//*****************************************************************************************
			// LECTURE
			//*****************************************************************************************
			
			/* --------------- Tags classiques --------------- */
			for(list<datatag>::iterator ldatatagIter = conf->GetList()->begin(); ldatatagIter != conf->GetList()->end(); ldatatagIter ++)
			{
				flagSendMessage = 0;
				
				//if ((ldatatagIter->GetMBType() == 0) && (ldatatagIter->GetMBType() == 1) && (ldatatagIter->GetMBType() == 2))
				//	continue;
				
				switch(ldatatagIter->GetMBType())
				{
					//INTS
					case 0:
						MBLength = 1;
						break;
					
					/*
					//DINT
					case 1:
						MBLength = 2;					
						break;
					*/
					
					//FLOAT
					case 2:
						MBLength = 2;					
						break;

					default:
						continue;
					//TODO A VERIFIER QUE CA MARCHE POUR LE FOR
						break;						
				}
				
				//---------------------------------------------------------------------------------
				rc_read = modbus_read_registers(ctx, ldatatagIter->GetMBAddress(), MBLength, tab_reg);
				//t = time(NULL);
				//ptrTagList[indexTagList].CurrentValue.dtTimeStamp = *localtime(&t);
				if (rc_read == -1) 
				{
					// Erreur de lecture
					// On ne touche pas à la valeur (c'est la dernière connue)
					// On change la quality
					// On flag message à sauvegarder
					ldatatagIter->SetQuality(Q_LASTKNOWNVALUE);
					flagSendMessage = 1;
					
					std::clog << kLogWarning << "mbserver_thread : modbus_read_registers | " << modbus_strerror(errno) << std::endl;
					ctxerror = -2;
				}
				else
				{
					ostringstream os;
					switch(ldatatagIter->GetMBType())
					{
						//INT
						case 0:
							tmpsigned = tab_reg[0];
							os << tmpsigned;
							sValue = os.str();
							if (sValue != ldatatagIter->GetValue())
							{
								ldatatagIter->SetValue(sValue);
								flagSendMessage = 1;
							}
							break;

						//FLOAT
						case 2:
							os << modbus_get_float(&tab_reg[0]);
							sValue = os.str();
							if (sValue != ldatatagIter->GetValue())
							{
								ldatatagIter->SetValue(sValue);
								flagSendMessage = 1;
							}
							break;					
					}
					
					if (ldatatagIter->GetQuality() != Q_GOOD)
						flagSendMessage = 1;
					
					ldatatagIter->SetQuality(Q_GOOD);
				}
				
				if (flagSendMessage > 0)
				{
					cqReadChange->push(*ldatatagIter);
					//ldatatagIter->ToString();
				}
				//---------------------------------------------------------------------------------					
			}
			
			/* --------------- Tags Adam --------------- */
			//TODO : améliorer le séquencement ?
			while(cqAdamReadToPLC->size() > 0)
			{
				adamtag tmp;
				cqAdamReadToPLC->pop(tmp);
							
				rc_read = modbus_read_registers(ctx, tmp.GetMBAddress(), 1, tab_reg);

				if (rc_read == -1) 
				{				
					std::clog << kLogWarning << "mbserver_thread : modbus_read_registers for Adam | " << modbus_strerror(errno) << std::endl;
					ctxerror = -2;
				}
				else
				{
					tmpsigned = tab_reg[0];
					if (tmpsigned != tmp.GetValue())
					{
						tmp.SetQueryType(-1);
						tmp.SetValue(tmpsigned);
						cqAdamResponseFromPLC->push(tmp);
					}
				}
			}

			//*****************************************************************************************
			// ECRITURE
			//*****************************************************************************************
			while(cqWriteRequest->size() > 0)
			{
				datatag tmp;
				cqWriteRequest->pop(tmp);
				
				
				ostringstream os;
				switch(tmp.GetMBType())
				{
					//INT
					case 0:
						MBLength = 1;
						tab_reg[0] = atoi(tmp.GetValue().c_str());
						rc_write = modbus_write_registers(ctx, tmp.GetMBAddress(),  MBLength, tab_reg);
						break;

					//FLOAT
					case 2:
						MBLength = 2;
						modbus_set_float(atof(tmp.GetValue().c_str()), &tab_reg[0]);
						rc_write = modbus_write_registers(ctx, tmp.GetMBAddress(),  MBLength, tab_reg);
						break;					
				}

				if (rc_write == -1) 
				{
					std::clog << kLogWarning << "mbserver_thread : modbus_write_registers | " << modbus_strerror(errno) << std::endl;
					ctxerror = -2;	
				}			
			}				
			//*****************************************************************************************
			
			sleep(1);
		}

		modbus_close(ctx);
	}
	
	modbus_free(ctx);	

	std::clog << kLogNotice << "mbserver_thread : end of thread" << std::endl;
	
	return;
}
