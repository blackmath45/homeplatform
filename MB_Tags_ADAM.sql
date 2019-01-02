-- Table: "MB_Tags_ADAM"

-- DROP TABLE "MB_Tags_ADAM";

CREATE TABLE "MB_Tags_ADAM"
(
  "ID" serial NOT NULL,
  "Nom" character varying(100),
  "PLC_ModBus_Address_Com" integer NOT NULL,
  "PLC_ModBus_Address_Input" integer NOT NULL,
  "PLC_ModBus_Address_Output" integer NOT NULL,
  CONSTRAINT "MB_Tags_ADAM_pkey" PRIMARY KEY ("ID")
)
WITH (
  OIDS=FALSE
);
ALTER TABLE "MB_Tags_ADAM"
  OWNER TO postgres;
