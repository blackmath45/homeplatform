-- Function: "SP_MB_Tags_WriteOrders_Get"()

-- DROP FUNCTION "SP_MB_Tags_WriteOrders_Get"();

CREATE OR REPLACE FUNCTION "SP_MB_Tags_WriteOrders_Get"()
  RETURNS TABLE("ID" integer, "ModBus_Address" integer, "ModBus_Type" integer, "Valeur" numeric) AS
$BODY$DECLARE

BEGIN

	RETURN QUERY

	SELECT	"MB_Tags_WriteOrders"."ID", 
		"MB_Tags_WriteOrders"."ModBus_Address", 
		"MB_Tags_WriteOrders"."ModBus_Type",
		"MB_Tags_WriteOrders"."Value"
	  FROM "MB_Tags_WriteOrders"
	  LEFT JOIN "EXT_Incoming" ON "MB_Tags_WriteOrders"."ID_Incoming" = "EXT_Incoming"."ID" AND "MB_Tags_WriteOrders"."Source" = 2
	  LEFT JOIN "SMS_Incoming" ON "MB_Tags_WriteOrders"."ID_Incoming" = "SMS_Incoming"."ID" AND "MB_Tags_WriteOrders"."Source" = 1
	  WHERE "MB_Tags_WriteOrders"."DoneStatus" = 0
	  AND CASE 
			WHEN "MB_Tags_WriteOrders"."Source" = 1 THEN "SMS_Incoming"."AddDT"
			WHEN "MB_Tags_WriteOrders"."Source" = 2 THEN "EXT_Incoming"."AddDT"
	       END > CURRENT_TIMESTAMP - ('2 hour')::interval
	  LIMIT 1;

	
END$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100
  ROWS 1000;
ALTER FUNCTION "SP_MB_Tags_WriteOrders_Get"()
  OWNER TO postgres;
 
