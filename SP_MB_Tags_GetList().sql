-- Function: "SP_MB_Tags_GetList"()

-- DROP FUNCTION "SP_MB_Tags_GetList"();

CREATE OR REPLACE FUNCTION "SP_MB_Tags_GetList"()
  RETURNS TABLE("ID" integer, "ModBus_Address" integer, "ModBus_Type" integer, "Valeur" numeric, "Quality" integer) AS
$BODY$DECLARE

query text;
tmpvaleur numeric;
tmpquality integer;

rec_tags RECORD;
cur_tags CURSOR
FOR 
SELECT tags."ID", tags."ModBus_Address", tags."ModBus_Type", tags."DestTable"
FROM "MB_Tags" tags;

BEGIN

	DROP TABLE IF EXISTS tmptags;

	CREATE TEMP TABLE tmptags
	(
		"ID" integer,
		"ModBus_Address" integer,
		"ModBus_Type" integer,
		"Valeur" numeric,
		"Quality" integer
	);

	OPEN cur_tags;
 
	LOOP
	
		FETCH cur_tags INTO rec_tags;
    
		EXIT WHEN NOT FOUND;

		SELECT 'SELECT "Valeur", "Quality" FROM "' || rec_tags."DestTable" || '" WHERE "DateHeure" = (SELECT MAX("DateHeure") FROM "' || rec_tags."DestTable" || '");' INTO query;

		EXECUTE query into tmpvaleur, tmpquality;
		insert into tmptags
			SELECT
				rec_tags."ID", 
				rec_tags."ModBus_Address", 
				rec_tags."ModBus_Type", 
				tmpvaleur,
				tmpquality;			
	
	END LOOP;
  

	CLOSE cur_tags;

	RETURN QUERY SELECT t."ID", t."ModBus_Address", t."ModBus_Type", t."Valeur", t."Quality" FROM tmptags t;
	
END$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100
  ROWS 1000;
ALTER FUNCTION "SP_MB_Tags_GetList"()
  OWNER TO postgres;
