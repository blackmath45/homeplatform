-- Function: "SP_MB_Tags_Update"(integer, integer, integer, text, integer)

-- DROP FUNCTION "SP_MB_Tags_Update"(integer, integer, integer, text, integer);

CREATE OR REPLACE FUNCTION "SP_MB_Tags_Update"(id integer, mbaddress integer, mbtype integer, value text, quality integer)
  RETURNS void AS
$BODY$DECLARE

query text;

rec_tags RECORD;

BEGIN


	SELECT tags."ID", tags."DestTable" INTO rec_tags
	FROM "MB_Tags" tags
	WHERE tags."ID" = ID;

	SELECT 'INSERT INTO "' || rec_tags."DestTable" || '" ("DateHeure", "Valeur", "Quality") VALUES (''' || CURRENT_TIMESTAMP::timestamp(0) with time zone || ''', ' || value || ', ' || quality || ');' INTO query;

	EXECUTE query;

END$BODY$
  LANGUAGE plpgsql VOLATILE
  COST 100;
ALTER FUNCTION "SP_MB_Tags_Update"(integer, integer, integer, text, integer)
  OWNER TO postgres;
 
