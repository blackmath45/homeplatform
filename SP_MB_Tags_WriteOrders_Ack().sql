 
-- Function: "SP_MB_Tags_WriteOrders_Ack"(integer)

-- DROP FUNCTION "SP_MB_Tags_WriteOrders_Ack"(integer);

CREATE OR REPLACE FUNCTION "SP_MB_Tags_WriteOrders_Ack"(id integer)
  RETURNS void AS
$BODY$DECLARE

BEGIN

	UPDATE "MB_Tags_WriteOrders"
	SET "MB_Tags_WriteOrders"."DoneDT"=current_timestamp, "MB_Tags_WriteOrders"."DoneStatus"=10
	WHERE "MB_Tags_WriteOrders"."ID"= id;
	
END$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION "SP_MB_Tags_WriteOrders_Ack"(integer)
  OWNER TO postgres;
