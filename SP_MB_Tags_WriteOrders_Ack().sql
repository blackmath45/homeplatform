-- Function: "SP_MB_Tags_WriteOrders_Ack"(integer)

-- DROP FUNCTION "SP_MB_Tags_WriteOrders_Ack"(integer);

CREATE OR REPLACE FUNCTION "SP_MB_Tags_WriteOrders_Ack"(id integer)
  RETURNS void AS
$BODY$DECLARE

BEGIN

	UPDATE "MB_Tags_WriteOrders"
	SET "DoneDT"=current_timestamp, "DoneStatus"=10
	WHERE "ID"= id;
	
END$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION "SP_MB_Tags_WriteOrders_Ack"(integer)
  OWNER TO postgres;
