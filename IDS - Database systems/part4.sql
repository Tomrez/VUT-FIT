-- 4. část - SQL skript pro vytvoření pokročilých objektů schématu databáze
--
-- Zadání - Evidence přestupků
--
-- @author Michal Blažek <xblaze38>
-- @author Tomáš Řezníček <xrezni33>


-- V databázi jsme pro převod reprezentace generalizace/specializace využili metodu tabulky pro nadtyp + pro podtypy s primárním klíčem nadtypu.
-- Tato metoda byla zvolena, abychom nemuseli kopírovat atributy jedné tabulky do dalších a tím ušetříme nějaké místo v paměti.

DROP TABLE Spachal CASCADE CONSTRAINTS;
DROP TABLE Prestupek CASCADE CONSTRAINTS;
DROP TABLE Ridic_ridi_motorove_vozidlo CASCADE CONSTRAINTS;
DROP TABLE Ridic CASCADE CONSTRAINTS;
DROP TABLE Osoba CASCADE CONSTRAINTS;
DROP TABLE Firma CASCADE CONSTRAINTS;
DROP TABLE Kradene CASCADE CONSTRAINTS;
DROP TABLE Motorove_vozidlo CASCADE CONSTRAINTS;
DROP TABLE Vlastnik CASCADE CONSTRAINTS;

CREATE TABLE Prestupek
(
    ID_prestupku            INT GENERATED AS IDENTITY PRIMARY KEY,
    Nazev                   VARCHAR(255) NOT NULL,
    Maximalni_vyse_pokuty   NUMBER(9,2) NOT NULL,
    Minimalni_vyse_pokuty   NUMBER(9,2),
    Popis                   VARCHAR(255) NOT NULL,
    CHECK (Minimalni_vyse_pokuty IS NULL OR Minimalni_vyse_pokuty <= Maximalni_vyse_pokuty)
);

CREATE TABLE Vlastnik
(
    ID_vlastnika    INT GENERATED AS IDENTITY PRIMARY KEY
);

CREATE TABLE Osoba
(
    ID_vlastnika    INT,
    Rodne_cislo     VARCHAR(11) PRIMARY KEY,
    Prijmeni        VARCHAR(255) NOT NULL,
    Jmeno           VARCHAR(255) NOT NULL,
    Trvaly_pobyt    VARCHAR(255) NOT NULL,
    Telefonni_cislo NUMBER(9),
    CHECK (LENGTH(TO_CHAR(Telefonni_cislo)) = 9 OR Telefonni_cislo IS NULL),
    CHECK (REGEXP_LIKE(Rodne_cislo, '^\d{2}[0156]\d[0-3]\d/\d{3,4}$')),
    FOREIGN KEY (ID_vlastnika) REFERENCES Vlastnik(ID_vlastnika) ON DELETE SET NULL
);

CREATE TABLE Firma
(
    ID_vlastnika    INT,
    ICO             NUMBER(8) PRIMARY KEY,
    Nazev           VARCHAR(255) NOT NULL,
    Sidlo           VARCHAR(255) NOT NULL,
    Telefonni_cislo NUMBER(9),
    CHECK (LENGTH(TO_CHAR(Telefonni_cislo)) = 9 OR Telefonni_cislo IS NULL),
    CHECK (LENGTH(ICO) = 8 AND SUBSTR(ICO, 8, 1) =
        (MOD((11 - MOD(
            8 * TO_NUMBER(SUBSTR(ICO, 1, 1)) + 7 * TO_NUMBER(SUBSTR(ICO, 2, 1)) + 6 * TO_NUMBER(SUBSTR(ICO, 3, 1)) + 5 * TO_NUMBER(SUBSTR(ICO, 4, 1)) +
            4 * TO_NUMBER(SUBSTR(ICO, 5, 1)) + 3 * TO_NUMBER(SUBSTR(ICO, 6, 1)) + 2 * TO_NUMBER(SUBSTR(ICO, 7, 1)), 11)
        ), 10))
    ),
    FOREIGN KEY (ID_vlastnika) REFERENCES Vlastnik(ID_vlastnika) ON DELETE CASCADE
);

CREATE TABLE Ridic
(
    Rodne_cislo         VARCHAR(11) PRIMARY KEY,
    Ridicske_opravneni  VARCHAR(255),
    FOREIGN KEY (Rodne_cislo) REFERENCES Osoba(Rodne_cislo) ON DELETE CASCADE
);

CREATE TABLE Spachal
(
    Rodne_cislo_ridice  VARCHAR(11),
    ID_prestupku        INT,
    Datum_spachani      DATE NOT NULL,
    Popis               VARCHAR(255) NOT NULL,
    Vyse_pokuty         NUMBER(9,2) NOT NULL,
    CHECK (Vyse_pokuty > 0),
    PRIMARY KEY (Rodne_cislo_ridice, ID_prestupku),
    FOREIGN KEY (Rodne_cislo_ridice) REFERENCES Ridic(Rodne_cislo) ON DELETE CASCADE,
    FOREIGN KEY (ID_prestupku) REFERENCES Prestupek(ID_prestupku) ON DELETE CASCADE
);

CREATE TABLE Motorove_vozidlo
(
    VIN                         VARCHAR(50) PRIMARY KEY NOT NULL,
    Znacka                      VARCHAR(255) NOT NULL,
    SPZ                         VARCHAR(8),
    Rok_vyroby                  NUMBER(4) NOT NULL,
    Ridicska_skupina            VARCHAR(2) NOT NULL,
    ID_vlastnika                INT,
    CHECK (Rok_vyroby > 1886),
    CHECK (Ridicska_skupina IN ('AM', 'A1', 'A2', 'A', 'B', 'B1', 'C', 'C1', 'D', 'D1', 'T')),
    CHECK (REGEXP_LIKE(VIN, '^[A-HJ-NPR-Z0-9]+$')),
    CHECK (LENGTH(SPZ) <= 8),
    CHECK (REGEXP_LIKE(SPZ, '^[A-Za-z0-9]+$')),
    CHECK (REGEXP_LIKE(SPZ, '[0-9]') AND REGEXP_LIKE(SPZ, '[A-Za-z]')),
    FOREIGN KEY (ID_vlastnika)  REFERENCES Vlastnik(ID_vlastnika) ON DELETE CASCADE
);

CREATE TABLE Ridic_ridi_motorove_vozidlo
(
    Rodne_cislo VARCHAR(11),
    VIN VARCHAR(50),
    PRIMARY KEY (Rodne_cislo, VIN),
    FOREIGN KEY (Rodne_cislo) REFERENCES Ridic(Rodne_cislo) ON DELETE CASCADE,
    FOREIGN KEY (VIN) REFERENCES Motorove_vozidlo(VIN) ON DELETE CASCADE
);

CREATE TABLE Kradene
(
    VIN                 VARCHAR(50) PRIMARY KEY,
    Datum_kradeze       DATE NOT NULL,
    Misto_kradeze       VARCHAR(255),
    CHECK (Datum_kradeze > DATE '1886-01-29'),
    FOREIGN KEY (VIN) REFERENCES Motorove_vozidlo(VIN) ON DELETE CASCADE
);

INSERT INTO Vlastnik (ID_vlastnika)
VALUES (DEFAULT);
INSERT INTO Vlastnik (ID_vlastnika)
VALUES (DEFAULT);
INSERT INTO Vlastnik (ID_vlastnika)
VALUES (DEFAULT);

INSERT INTO Firma (ID_vlastnika, ICO, Nazev, Sidlo, Telefonni_cislo) VALUES
(1, 25596641, 'Redhat', 'Brno', NULL);
INSERT INTO Firma (ID_vlastnika, ICO, Nazev, Sidlo, Telefonni_cislo) VALUES
(3, 15596648, 'FirmaVPraze', 'Praha', 123456789);

INSERT INTO Osoba (ID_vlastnika, Rodne_cislo, Prijmeni, Jmeno, Trvaly_pobyt, Telefonni_cislo)
VALUES (2, '120430/0789', 'Novak', 'Petr', 'Praha', 111222333);
INSERT INTO Osoba (              Rodne_cislo, Prijmeni, Jmeno, Trvaly_pobyt, Telefonni_cislo)
VALUES (   '915624/0321', 'Svobodova', 'Anna', 'Brno', NULL);
INSERT INTO Osoba (              Rodne_cislo, Prijmeni, Jmeno, Trvaly_pobyt, Telefonni_cislo)
VALUES (   '111111/1111', 'Reznicek', 'Tomas', 'Jihlava', NULL);

INSERT INTO Ridic (Rodne_cislo, Ridicske_opravneni) VALUES
('120430/0789', 'B');
INSERT INTO Ridic (Rodne_cislo, Ridicske_opravneni) VALUES
('915624/0321', 'A2');

INSERT INTO Motorove_vozidlo (VIN, Znacka, SPZ, Rok_vyroby, Ridicska_skupina, ID_vlastnika) VALUES
('ABC123', 'Kawasaki', 'ABC1234', 2020, 'A2', 3);
INSERT INTO Motorove_vozidlo (VIN, Znacka, SPZ, Rok_vyroby, Ridicska_skupina,  ID_vlastnika) VALUES
('XYZ789', 'Volkswagen', 'XYZ9876', 2005, 'B', 2);
INSERT INTO Motorove_vozidlo (VIN, Znacka, SPZ, Rok_vyroby, Ridicska_skupina,  ID_vlastnika) VALUES
('XYZ123', 'Skoda', 'XYZ12345', 2009, 'B', 1);
INSERT INTO Motorove_vozidlo (VIN, Znacka, SPZ, Rok_vyroby, Ridicska_skupina, ID_vlastnika) VALUES
('LAL420', 'BMW', 'LOL6969', 2018, 'A', 1);

INSERT INTO Ridic_ridi_motorove_vozidlo (Rodne_cislo, VIN) VALUES
('120430/0789', 'ABC123');
INSERT INTO Ridic_ridi_motorove_vozidlo (Rodne_cislo, VIN) VALUES
('915624/0321', 'XYZ789');

INSERT INTO Prestupek (Nazev, Maximalni_vyse_pokuty, Minimalni_vyse_pokuty, Popis) VALUES
('Překročení rychlosti', 100000.00, 5000.00, 'Ve městě minimálně o 5 km/h. Mimo město minimálně o 10 km/h.');
INSERT INTO Prestupek (Nazev, Maximalni_vyse_pokuty, Minimalni_vyse_pokuty, Popis) VALUES
('Zákaz parkování', 70000.00, NULL, 'Zóna A a B není pro návštěvníky a zóna C je pouze od 6:00 do 17:00.');
INSERT INTO Prestupek (Nazev, Maximalni_vyse_pokuty, Minimalni_vyse_pokuty, Popis) VALUES
('Dopravní nehoda', 7000.00, 4000.00, 'Řidič spáchal dopravní nehodu');
INSERT INTO Prestupek (Nazev, Maximalni_vyse_pokuty, Minimalni_vyse_pokuty, Popis) VALUES
('Nezastavení na semaforu', 8000.00, 7500.00, 'Řidič projel semaforem i přes svítící červené světlo');

INSERT INTO Spachal (Rodne_cislo_ridice, ID_prestupku, Datum_spachani, Popis, Vyse_pokuty) VALUES
('915624/0321', 1, TO_DATE('2024-03-22', 'YYYY-MM-DD'), 'Překročení rychlosti', 6969.69);

INSERT INTO Spachal (Rodne_cislo_ridice, ID_prestupku, Datum_spachani, Popis, Vyse_pokuty) VALUES
('915624/0321', 2, TO_DATE('2024-03-22', 'YYYY-MM-DD'), 'Překročení rychlosti', 6969.69);

INSERT INTO Spachal (Rodne_cislo_ridice, ID_prestupku, Datum_spachani, Popis, Vyse_pokuty) VALUES
('120430/0789', 2, TO_DATE('2024-03-20', 'YYYY-MM-DD'), 'Překročení rychlosti', 69420.69);

INSERT INTO Kradene (VIN, Datum_kradeze, Misto_kradeze) VALUES
('ABC123', TO_DATE('2024-03-23', 'YYYY-MM-DD'), 'Ostrava');

COMMIT;

--SELECT vypise informace o osobach a jejich ridicskych opravnenich
SELECT O.Jmeno, O.Prijmeni, R.Ridicske_opravneni
FROM Osoba O
NATURAL JOIN Ridic R;

--SELECT vypise informace o kradenych autech, ktera byla vyrobena po zadanem roce
SELECT K.VIN, MV.Znacka, K.Datum_kradeze, K.Misto_kradeze
FROM Kradene K
JOIN Motorove_vozidlo MV ON K.VIN = MV.VIN
WHERE MV.Rok_vyroby > 2010;

-- SELECT vypise informace o motorovych vozidlech vlastnenych Osobou s danym rodnym cislem
SELECT MV.VIN, MV.Znacka as Znacka_auta, MV.SPZ, O.Jmeno || ' ' || O.Prijmeni as Jmeno_vlastnika
FROM Motorove_vozidlo MV
JOIN Vlastnik V on MV.ID_vlastnika = V.ID_vlastnika
JOIN Osoba O on V.ID_vlastnika = O.ID_vlastnika
WHERE O.Rodne_cislo = '120430/0789';


-- SELECT vypise kolik prestupku kazdy z ridicu spachal
SELECT S.Rodne_cislo_ridice, O.Jmeno || ' ' || O.Prijmeni as Cele_jmeno, COUNT(*) AS Pocet_spachanych_prestupku
FROM Spachal S
JOIN Osoba O ON S.Rodne_cislo_ridice = O.Rodne_cislo
GROUP BY S.Rodne_cislo_ridice, O.Jmeno, O.Prijmeni;


-- SELECT vypise celkovou castku, kterou kazdy z ridicu zaplatil na pokutach
SELECT R.Rodne_cislo, O.Jmeno || ' ' || O.Prijmeni AS Cele_jmeno, SUM(S.Vyse_pokuty) AS Celkova_vyse_pokut
FROM Ridic R
JOIN Osoba O ON R.Rodne_cislo = O.Rodne_cislo
JOIN Spachal S ON R.Rodne_cislo = S.Rodne_cislo_ridice
GROUP BY R.Rodne_cislo, O.Jmeno, O.Prijmeni;

-- SELECT vypise osoby, ktere spachaly nejaky prestupek
SELECT O.Rodne_cislo, O.Jmeno, O.Prijmeni
FROM Osoba O
WHERE EXISTS (
    SELECT *
    FROM Spachal S
    WHERE S.Rodne_cislo_ridice = O.Rodne_cislo
);

-- SELECT vypise osoby, ktere maji pouze zadane ridicske opravneni a zadne jine
SELECT O.Rodne_cislo, O.Jmeno, O.Prijmeni
FROM Ridic R
JOIN Osoba O ON R.Rodne_cislo = O.Rodne_cislo
WHERE Ridicske_opravneni = 'A2'
AND NOT EXISTS (
    SELECT *
    FROM Ridic R2
    WHERE R2.Rodne_cislo = R.Rodne_cislo
    AND R2.Ridicske_opravneni <> 'A2'
);

-- SELECT vypise prestupky spachane danym ridicem
SELECT *
FROM Prestupek P
WHERE P.ID_prestupku IN (
    SELECT S.ID_prestupku
    FROM Spachal S
    JOIN Ridic R ON S.Rodne_cislo_ridice = R.Rodne_cislo
    JOIN Ridic_ridi_motorove_vozidlo RR ON S.Rodne_cislo_ridice = RR.Rodne_cislo
    JOIN Motorove_vozidlo MV ON RR.VIN = MV.VIN
    WHERE RR.Rodne_cislo = '120430/0789'
);

UPDATE Prestupek
SET Minimalni_vyse_pokuty = 0
WHERE Minimalni_vyse_pokuty IS NULL;

COMMIT;


-- Prida prava xrezni33
GRANT ALL PRIVILEGES ON XBLAZE38.FIRMA TO XREZNI33 WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON XBLAZE38.KRADENE TO XREZNI33 WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON XBLAZE38.MOTOROVE_VOZIDLO TO XREZNI33 WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON XBLAZE38.OSOBA TO XREZNI33 WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON XBLAZE38.PRESTUPEK TO XREZNI33 WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON XBLAZE38.RIDIC TO XREZNI33 WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON XBLAZE38.RIDIC_RIDI_MOTOROVE_VOZIDLO TO XREZNI33 WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON XBLAZE38.SPACHAL TO XREZNI33 WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON XBLAZE38.VLASTNIK TO XREZNI33 WITH GRANT OPTION;

-- Trigger kontroluje vysi pokuty, zda je v rozsahu povolenem pro dany prestupek
CREATE OR REPLACE TRIGGER zkontroluj_vysi_pokuty
    BEFORE INSERT ON Spachal
    FOR EACH ROW
DECLARE
    Min_pokuta Prestupek.Minimalni_vyse_pokuty%TYPE;
    Max_pokuta Prestupek.Maximalni_vyse_pokuty%TYPE;
BEGIN
    IF :NEW.Vyse_pokuty < Min_pokuta OR :NEW.Vyse_pokuty > Max_pokuta THEN
        RAISE_APPLICATION_ERROR(-20204, 'Udelena pokuta neni v rozsahu povolene pokuty za tento prestupek!');
    END IF;
END;

-- Test triggeru, ktery by mel byt uspesne vykonan
INSERT INTO Spachal (Rodne_cislo_ridice, ID_prestupku, Datum_spachani, Popis, Vyse_pokuty)
VALUES ('915624/0321', 3, TO_DATE('2024-03-21', 'YYYY-MM-DD'), 'Auticko udelalo baaac', 6969.69);
COMMIT;

-- Test triggeru, ktery by nemel byt vykonan kvuli nespravne vysi pokuty
INSERT INTO Spachal(Rodne_cislo_ridice, ID_prestupku, Datum_spachani, Popis, Vyse_pokuty)
VALUES ('915624/0321', 4, TO_DATE('2024-03-21', 'YYYY-MM-DD'), 'Auticko nezastavilo na semaforech ale dostalo spatnou vysi pokuty', 696969.69);
COMMIT;



-- Trigger kontroluje ridicske opraveni ridice na rizeni daneho vozidla
CREATE OR REPLACE TRIGGER zkontroluj_ridicske_opravneni
    BEFORE INSERT ON Ridic_ridi_motorove_vozidlo
    FOR EACH ROW
DECLARE
    Ridicska_skupina_vozidla Motorove_vozidlo.Ridicska_skupina%TYPE;
    Ridicske_opravneni_ridice Ridic.Ridicske_opravneni%TYPE;
BEGIN
    SELECT Ridicska_skupina
    INTO Ridicska_skupina_vozidla
    FROM Motorove_vozidlo
    WHERE VIN = :NEW.VIN;

    SELECT Ridicske_opravneni
    INTO Ridicske_opravneni_ridice
    FROM Ridic
    WHERE Rodne_cislo = :NEW.Rodne_cislo;

    IF Ridicska_skupina_vozidla <> Ridicske_opravneni_ridice THEN
        RAISE_APPLICATION_ERROR(-20201, 'Ridic nema opravneni na rizeni daneho vozidla!');
    END IF;
END;
COMMIT;

-- Pouziti triggeru pro kontrolu spravnosti ridicskeho opravneni na rizeni daneho vozidla se spravnymi hodnotami
INSERT INTO Ridic_ridi_motorove_vozidlo (Rodne_cislo, VIN) VALUES
('120430/0789', 'XYZ123');
COMMIT;

-- Pouziti triggeru pro kontrolu spravnosti ridicskeho opravneni na rizeni daneho vozidla se spatnymi hodnotami
INSERT INTO Ridic_ridi_motorove_vozidlo (Rodne_cislo, VIN) VALUES
('120430/0789', 'LAL420');
COMMIT;


-- Komplexni SELECT s WITH a CASE
-- Dotaz spocita pocet prestupku pro vsechny ridice
WITH Pocet_prestupku_ridice AS
(
    SELECT S.Rodne_cislo_ridice, COUNT(*) AS Pocet_prestupku
    FROM Spachal S
    GROUP BY S.Rodne_cislo_ridice
)

-- Dotaz vypise vsechny osoby a jejich pocet spachanych prestupku
SELECT O.Jmeno, O.Prijmeni,
    CASE
        WHEN P.Rodne_cislo_ridice IS NOT NULL THEN
            P.Pocet_prestupku
        ELSE 0
    END AS Pocet_prestupku_osoby
FROM Osoba O LEFT JOIN Ridic R ON O
    .Rodne_cislo = R.Rodne_cislo LEFT JOIN Pocet_prestupku_ridice P ON R.Rodne_cislo = P.Rodne_cislo_ridice;


-- Procedura pro vlozeni noveho prestupku k ridici
CREATE OR REPLACE PROCEDURE Vlozit_spachany_prestupek_ridici
(
    Rodne_cislo_ridice_procedura VARCHAR,
    ID_prestupku_procedura INT,
    Datum_spachani_procedura DATE,
    Popis_procedura VARCHAR,
    Vyse_pokuty_procedura NUMBER
)
AS
    Rodne_cislo_if_exist INT;
    Prestupek_if_exist INT;
BEGIN
    -- Zjisteni, zda rodne cislo ridice existuje
    SELECT COUNT(*)
    INTO Rodne_cislo_if_exist
    FROM Ridic
    WHERE Rodne_cislo = Rodne_cislo_ridice_procedura;

    -- Zjisteni, zda prestupek existuje
    SELECT COUNT(*)
    INTO Prestupek_if_exist
    FROM Prestupek
    WHERE ID_prestupku = ID_prestupku_procedura;

    -- Vyjimka ze rodne cislo neexistuje nebo prestupek neexistuje
    IF Rodne_cislo_if_exist = 0 OR Prestupek_if_exist = 0 THEN
        RAISE_APPLICATION_ERROR(-20202, 'Zadane rodne cislo nebo ID prestupku neexistuje!');
    ELSE
        INSERT INTO Spachal (Rodne_cislo_ridice, ID_prestupku, Datum_spachani, Popis, Vyse_pokuty)
        VALUES (Rodne_cislo_ridice_procedura, ID_prestupku_procedura, Datum_spachani_procedura, Popis_procedura, Vyse_pokuty_procedura);
        DBMS_OUTPUT.put_line('Vlozil se novy prestupek k ridici.');
    END IF;
EXCEPTION
    WHEN OTHERS THEN
        DBMS_OUTPUT.put_line('Chyba: ' || SQLERRM);
END;


-- Volani procedury pro vlozeni spachaneho prestupku
BEGIN
    Vlozit_spachany_prestupek_ridici('120430/0789', 1, TO_DATE('2024-04-21', 'YYYY-MM-DD'), 'Bum bac', 20000.68);
END;
COMMIT;

-- Volani procedury se spatnymy vstupnimy daty - Spatne RC
BEGIN
    Vlozit_spachany_prestupek_ridici('120439/0789', 1, TO_DATE('2024-04-21', 'YYYY-MM-DD'), 'Bum bac', 20000.68);
END;
COMMIT;

-- Procedura pro odstraneni prestupku od ridice, pokud treba zamestnanec pridal prestupek nespravnemu ridicovi
CREATE OR REPLACE PROCEDURE Odstranit_prestupek_ridice
(
    Rodne_cislo_ridice_procedura VARCHAR,
    ID_prestupku_procedura INT
)
AS
    Rodne_cislo_if_exist INT;
    Prestupek_if_exist INT;
    Prestupek_cursor SYS_REFCURSOR;
    Rodne_cislo_cursor SYS_REFCURSOR;
BEGIN
    -- Kurzor pro prestupek
    OPEN Prestupek_cursor FOR
        SELECT COUNT(*)
        FROM Prestupek
        WHERE ID_prestupku = ID_prestupku_procedura;

    -- Kurzor pro rodne cislo
    OPEN Rodne_cislo_cursor FOR
        SELECT COUNT(*)
        FROM Ridic
        WHERE Rodne_cislo = Rodne_cislo_ridice_procedura;

    -- Fetching data from cursors
    FETCH Prestupek_cursor INTO Prestupek_if_exist;
    FETCH Rodne_cislo_cursor INTO Rodne_cislo_if_exist;

    -- Close cursors
    CLOSE Prestupek_cursor;
    CLOSE Rodne_cislo_cursor;

    -- Vyhozeni vyjimky pokud nemame rodne cislo ridice nebo ID prestupku
    IF Prestupek_if_exist = 0 OR Rodne_cislo_if_exist = 0 THEN
        RAISE_APPLICATION_ERROR(-20203, 'Chybne ID prestupku nebo rodne cislo ridice!');
    ELSE
        DELETE FROM Spachal
        WHERE ID_prestupku = ID_prestupku_procedura
        AND Rodne_cislo_ridice = Rodne_cislo_ridice_procedura;

        DBMS_OUTPUT.put_line('Prestupek byl ridici odebran.');
    END IF;
EXCEPTION
    WHEN OTHERS THEN
        DBMS_OUTPUT.put_line('Chyba: ' || SQLERRM);
END;

-- Pouziti procedury pro odstraneni spachaneho prestupku s korektnimi vstupnimi daty
BEGIN
    Odstranit_prestupek_ridice('915624/0321', 1);
END;
COMMIT;

-- Pouziti procedury pro odstraneni spachaneho prestupku s nespravnymi vstupnimi daty
BEGIN
    Odstranit_prestupek_ridice('015624/0321', 1);
END;
COMMIT;

-- pridani prav xrezni33 na procedury
GRANT ALL PRIVILEGES ON XBLAZE38.ODSTRANIT_PRESTUPEK_RIDICE TO XREZNI33 WITH GRANT OPTION;
GRANT ALL PRIVILEGES ON XBLAZE38.VLOZIT_SPACHANY_PRESTUPEK_RIDICI TO XREZNI33 WITH GRANT OPTION;


-- EXPLAIN PLAN bez indexu (SELECT vypise kolik prestupku kazdy z ridicu spachal)
EXPLAIN PLAN FOR
    SELECT S.Rodne_cislo_ridice, O.Jmeno || ' ' || O.Prijmeni as Cele_jmeno, COUNT(*) AS Pocet_spachanych_prestupku
    FROM Spachal S
    JOIN Osoba O ON S.Rodne_cislo_ridice = O.Rodne_cislo
    GROUP BY S.Rodne_cislo_ridice, O.Jmeno, O.Prijmeni;

SELECT plan_table_output
FROM TABLE(dbms_xplan.display());

-- INDEX vytvori index Rodne_cislo_index na sloupci Rodne_cislo_ridice
CREATE INDEX Rodne_cislo_index ON Spachal(Rodne_cislo_ridice);

-- EXPLAIN PLAN s indexem (SELECT vypise kolik prestupku kazdy z ridicu spachal)
EXPLAIN PLAN FOR
    SELECT /*+ INDEX(Spachal Rodne_cislo_index)*/ S.Rodne_cislo_ridice, O.Jmeno || ' ' || O.Prijmeni as Cele_jmeno, COUNT(*) AS Pocet_spachanych_prestupku
    FROM Spachal S
    JOIN Osoba O ON S.Rodne_cislo_ridice = O.Rodne_cislo
    GROUP BY S.Rodne_cislo_ridice, O.Jmeno, O.Prijmeni;

SELECT plan_table_output
FROM TABLE(dbms_xplan.display());

-- Materializovany pohled na informace o kradenych vozidlech od xrezni33
CREATE MATERIALIZED VIEW LOG ON XBLAZE38.KRADENE;
CREATE MATERIALIZED VIEW LOG ON XBLAZE38.MOTOROVE_VOZIDLO;

DROP MATERIALIZED VIEW XREZNI33.Kradena_vozidla;

-- Tato cast kodu se spousti u XREZNI33
/*
CREATE MATERIALIZED VIEW XREZNI33.Kradena_vozidla
    nologging
    cache
    build immediate
    enable query rewrite
    AS
        SELECT MV.*, K.DATUM_KRADEZE, K.MISTO_KRADEZE
        FROM XBLAZE38.KRADENE K
        JOIN XBLAZE38.MOTOROVE_VOZIDLO MV ON MV.VIN = K.VIN;
*/
COMMIT;
