#include "adam6x50_thread.h"

void adam6x50_thread::SetConnectionString(string sConnectionString) { this->sConnectionString = sConnectionString; }
void adam6x50_thread::SetQueueAdamReadToPLC(cqueue<adamtag> *cqReadToPLC) { this->cqReadToPLC = cqReadToPLC; }
void adam6x50_thread::SetQueueAdamResponseFromPLC(cqueue<adamtag> *cqResponseFromPLC) { this->cqResponseFromPLC = cqResponseFromPLC; }
void adam6x50_thread::SetQueueAdamWriteToPLC(cqueue<adamtag> *cqWriteToPLC) { this->cqWriteToPLC = cqWriteToPLC; }

void adam6x50_thread::ThreadStart()
{
	thargs.cConnectionString = strdup(this->sConnectionString.c_str());
	thargs.cqReadToPLC = this->cqReadToPLC;
	thargs.cqResponseFromPLC = this->cqResponseFromPLC;
	thargs.cqWriteToPLC = this->cqWriteToPLC;

	std::clog << kLogNotice << "adam6x50_thread : starting thread" << std::endl;
	pthread_create(&this->thread_id, NULL, &adam6x50_thread::ThreadWrapper, (void *)&thargs);
}

void adam6x50_thread::ThreadJoin()
{
	pthread_join(this->thread_id, NULL);
	std::clog << kLogNotice << "adam6x50_thread : thread join" << std::endl;
}

void adam6x50_thread::threadFunction(void* args)
{
	std::clog << kLogNotice << "adam6x50_thread : thread started" << std::endl;
	
	//*************** ARGS ***************
	struct threadArgsStruct *thargs = (struct threadArgsStruct*)args;
	char *cConnectionString;
	cConnectionString = (char *)thargs->cConnectionString;
	string sConnectionString(cConnectionString);	

	cqueue<adamtag> *cqReadToPLC = (cqueue<adamtag> *)thargs->cqReadToPLC;
	cqueue<adamtag> *cqResponseFromPLC = (cqueue<adamtag> *)thargs->cqResponseFromPLC;
	cqueue<adamtag> *cqWriteToPLC = (cqueue<adamtag> *)thargs->cqWriteToPLC;
	//************************************
	
	
	/* --------------- ADAM SOCKET --------------- */
	struct sockaddr_in si_me, si_other;

	int s;
	unsigned int slen = sizeof(si_other);
	int recv_len;
	int send_len;
	char recvbuff[BUFLEN];
	char sendbuff[BUFLEN];
    
	int comcheck = 0;
	int istate = 0;
	int ostate = 0;
	
	int msgrecv = 0;
	/* --------------- ADAM SOCKET --------------- */	

	adam6x50_conf *conf = new adam6x50_conf();

	conf->SetConnectionString(sConnectionString);
	conf->LoadConf();

	
	//*****************************************************************************************
	// Début Gestion ADAM6x50
	//*****************************************************************************************
	
	//create a UDP socket
	if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		std::clog << kLogWarning << "adam6x50_thread : Socket failed | " << std::endl;
		//TODO Ne pas sortir du thread et faire un retry ?
		return;
	}
	
	struct timeval read_timeout;
	read_timeout.tv_sec = 0;
	read_timeout.tv_usec = 250;
	setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);	
     
	// zero out the structure
	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
     
	//bind socket to port
	if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
	{
		std::clog << kLogWarning << "adam6x50_thread : Bind failed | " << std::endl;
		//TODO Ne pas sortir du thread et faire un retry ?
		return;
	}

	std::clog << kLogNotice << "adam6x50_thread : starting main loop" << std::endl;
	
	while(1)
	{
		/* --------------- Response Lecture Input et Output --------------- */
		while(cqResponseFromPLC->size() > 0)
		{
			adamtag tmp;
			cqResponseFromPLC->pop(tmp);

			if(tmp.GetMBAddress() == conf->GetPLCMBAddress_Input())
				istate = tmp.GetValue();
			
			if(tmp.GetMBAddress() == conf->GetPLCMBAddress_Output())
				ostate = tmp.GetValue();
			
			printf("---------- istate or ostate change ----------\n");
		}
		
		/* --------------- Query Lecture Input et Output --------------- */
		// Test si la queue de demande est vide. Si oui on insert une demande
		if(cqReadToPLC->size() == 0)
		{
			adamtag queryInput;
			adamtag queryOuput;	
			queryInput.SetQueryType(1);
			queryOuput.SetQueryType(1);
			queryInput.SetMBAddress(conf->GetPLCMBAddress_Input());
			queryOuput.SetMBAddress(conf->GetPLCMBAddress_Output());
			queryInput.SetValue(istate);
			queryOuput.SetValue(ostate);
			
			cqReadToPLC->push(queryInput);
			cqReadToPLC->push(queryOuput);
		}
		
		if ((recv_len = recvfrom(s, recvbuff, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) == -1)
		{
			//perror("Error : recvfrom() failed !\n");
			//return NULL;			
		}
		else
		{
			msgrecv = 1;
		}

		sleep(1);
		
		/*
		while (mqTagAdamInputRequestAttr.mq_curmsgs > 0)
		{
			ssize_t len_recv;
			len_recv = mq_receive(mqTagAdamInputRequest, mqTagAdamInputRequestBuffer, mqTagAdamInputRequestBufferLength, NULL);
			if (len_recv == -1)
			{
				perror("ADAM6250 : mq_receive : ");
				return;
			}
						
			if(len_recv == sizeof(sWriteValue))
			{
				sWriteValue tmp;
				memcpy(&tmp, mqTagAdamInputRequestBuffer, sizeof(tmp));	
				
				for(j = 0; j < lengthTagList; j++)
				{
						if(ptrTagList[j].PLC_ModBus_Address == tmp.PLC_ModBus_Address)
						{
							if(ptrTagList[j].RO_or_RW == 1)
							{
								if(ptrTagList[j].DEVICE_ModBus_Address == 1)
									istate = tmp.PLC_ModBus_Value;
							}
							break;
						}		
				}	
			}
					
			if (mq_getattr(mqTagAdamInputRequest, &mqTagAdamInputRequestAttr) != 0) 
			{
				perror("mq_getattr");
				return;
			}				
		}		*/	

		/*
		if (msgrecv > 0)
		{
			comcheck++;
			
			//ENVOI comcheck à API via modbus
		
			//printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
			//printf("Data: %s\n" , recvbuff);
			
			//Test PW       
			if (strstr(recvbuff, "$01PW0") != NULL)
			{
				sprintf(sendbuff,">01\r");
				send_len = strlen(sendbuff);

				if (sendto(s, sendbuff, send_len, 0, (struct sockaddr*) &si_other, slen) == -1)
				{
					perror("Error : sendto() failed !\n");
					return NULL;	
				}
			}
			
			//Lecture IO
			if (strstr(recvbuff, "$016") != NULL)
			{
				int tmp = 0;
				tmp = istate + (ostate<<8);
						
				sprintf(sendbuff,"!01%06X\r",tmp);
				send_len = strlen(sendbuff);

				if (sendto(s, sendbuff, send_len, 0, (struct sockaddr*) &si_other, slen) == -1)
				{
					perror("Error : sendto() failed !\n");
					return NULL;	
				}
			}
			
			//Ecriture Output
			if (strstr(recvbuff, "#01") != NULL)
			{
				int value;
				int channel;
				char cvalue[3];
				char cchannel[2];

				memcpy(cvalue, &recvbuff[5], 2);
				cvalue[2] = '\0';			

				memcpy(cchannel, &recvbuff[4], 1);
				cchannel[1] = '\0';			
				
				value = atoi(cvalue);
				channel = atoi(cchannel);
				
				if (value > 0)
					ostate = ostate | (1<<channel);
				else
					ostate = ostate&(127-(1<<channel));
				
				sprintf(sendbuff,">\r");
				send_len = strlen(sendbuff);

				if (sendto(s, sendbuff, send_len, 0, (struct sockaddr*) &si_other, slen) == -1)
				{
					perror("Error : sendto() failed !\n");
					return NULL;	
				}
				
				for(indexTagList = 0; indexTagList < lengthTagList; indexTagList++)
				{			
					if(ptrTagList[indexTagList].DEVICE_ModBus_Address == 2)
					{
						sReadValue tmp;

						tmp.PLC_ModBus_Address = ptrTagList[indexTagList].PLC_ModBus_Address;
						tmp.PLC_ModBus_Value = ostate;
						
						mq_send(mqTagReadUpdate, (const char *) &tmp, sizeof(tmp), 0);	

						break;
					}
				}			
			}		

			//print details of the client/peer and the data received
			//printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
			//printf("Data: %s\n" , recvbuff);
		}
		
		msgrecv = 0;
		*/
		//printf("---------- MB ADAM6250 Tick ----------\n");
		
		/* --------------- Ecriture Output --------------- */
	}
	
	std::clog << kLogNotice << conf->GetPLCMBAddress_Com() << std::endl;
	
	//*****************************************************************************************
	// Fin Gestion ADAM6x50
	//*****************************************************************************************

	std::clog << kLogNotice << "adam6x50_thread : end of thread" << std::endl;
	
	pthread_exit(NULL);
	return;
}