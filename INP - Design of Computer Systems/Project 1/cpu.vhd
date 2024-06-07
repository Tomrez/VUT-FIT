-- cpu.vhd: Simple 8-bit CPU (BrainFuck interpreter)
-- Copyright (C) 2023 Brno University of Technology,
--                    Faculty of Information Technology
-- Author(s): Tomas Reznicek <xrezni33@stud.fit.vutbr.cz>
--
library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_arith.all;
use ieee.std_logic_unsigned.all;

-- ----------------------------------------------------------------------------
--                        Entity declaration
-- ----------------------------------------------------------------------------
entity cpu is
 port (
   CLK   : in std_logic;  -- hodinovy signal
   RESET : in std_logic;  -- asynchronni reset procesoru
   EN    : in std_logic;  -- povoleni cinnosti procesoru
 
   -- synchronni pamet RAM
   DATA_ADDR  : out std_logic_vector(12 downto 0); -- adresa do pameti
   DATA_WDATA : out std_logic_vector(7 downto 0); -- mem[DATA_ADDR] <- DATA_WDATA pokud DATA_EN='1'
   DATA_RDATA : in std_logic_vector(7 downto 0);  -- DATA_RDATA <- ram[DATA_ADDR] pokud DATA_EN='1'
   DATA_RDWR  : out std_logic;                    -- cteni (0) / zapis (1)
   DATA_EN    : out std_logic;                    -- povoleni cinnosti
   
   -- vstupni port
   IN_DATA   : in std_logic_vector(7 downto 0);   -- IN_DATA <- stav klavesnice pokud IN_VLD='1' a IN_REQ='1'
   IN_VLD    : in std_logic;                      -- data platna
   IN_REQ    : out std_logic;                     -- pozadavek na vstup data
   
   -- vystupni port
   OUT_DATA : out  std_logic_vector(7 downto 0);  -- zapisovana data
   OUT_BUSY : in std_logic;                       -- LCD je zaneprazdnen (1), nelze zapisovat
   OUT_WE   : out std_logic;                      -- LCD <- OUT_DATA pokud OUT_WE='1' a OUT_BUSY='0'

   -- stavove signaly
   READY    : out std_logic;                      -- hodnota 1 znamena, ze byl procesor inicializovan a zacina vykonavat program
   DONE     : out std_logic                       -- hodnota 1 znamena, ze procesor ukoncil vykonavani programu (narazil na instrukci halt)
 );
end cpu;


-- ----------------------------------------------------------------------------
--                      Architecture declaration
-- ----------------------------------------------------------------------------
architecture behavioral of cpu is

 -------------- PC (program counter) -------------
    
  signal pc_out: std_logic_vector (12 downto 0);
  signal pc_inc: std_logic;
  signal pc_dec: std_logic;


 -------------- PTR (pointer) --------------------

  signal ptr_out: std_logic_vector (12 downto 0);
  signal ptr_inc: std_logic;
  signal ptr_dec: std_logic;
  signal ptr_reset: std_logic;


 -------------- CNT (counter) --------------------

  signal cnt_out: std_logic_vector(7 downto 0);
  signal cnt_inc: std_logic;
  signal cnt_dec: std_logic;


 -------------- STATES ----------------------------
 
  type fsm_state is (

  -- zacatecni stav - vicemene init
  s_start,

  -- stavy pro pripravu pred dekodovanim
  s_fetch_before,
  s_fetch,

  -- stav, ve kterem dekodujeme jednoltive znaky programu
  s_decode,

  -- move
  s_ptr_inc,
  s_ptr_dec,

  -- inc
  s_val_inc,
  s_val_inc_2,
  s_val_inc_3,

  -- dec
  s_val_dec,
  s_val_dec_2,
  s_val_dec_3,

  -- leva zavorka while
  s_while_start_before,
  s_while_start,
  s_while_start_2,

  -- sekvence pro opusteni while cyklu
  s_while_go_exit,
  s_while_go_exit_2,
  s_while_go_exit_xx,

  -- prava zavorka while s kontrolou platnosti podminky
  s_while_end,
  s_while_end_2,
  s_while_end_3,
  s_while_end_4,
  s_while_end_5,

  -- znak pro breaknuti while cyklu
  s_break,
  s_break_2,

  -- vypis znaku
  s_write_possible,
  s_write,

  --nacteni hodnoty
  s_get,
  s_get_2,

  -- ukonceni programu
  s_halt
 
  );
  -- aktualni a nasledujici stav, defaultne stav start
  signal state : fsm_state := s_start;
  signal nState : fsm_state := s_start;

  ---------------- MUX 1  ---------------

    --vybira ze dvou hodnot -> staci nam jednobitova hodnota
    signal mx1Sel: std_logic;
    signal mx1Out: std_logic_vector (12 downto 0) := (others => '0');


  ---------------- MUX 2  ---------------

  -- vybira ze tri hodnot -> bude potreba vektor o velikosti dva bity
  signal mx2Sel: std_logic_vector (1 downto 0) := (others => '0');
  signal mx2Out: std_logic_vector (7 downto 0) := (others => '0');    



begin
  -- pri tvorbe kodu reflektujte rady ze cviceni INP, zejmena mejte na pameti, ze 
  --   - nelze z vice procesu ovladat stejny signal,
  --   - je vhodne mit jeden proces pro popis jedne hardwarove komponenty, protoze pak
  --      - u synchronnich komponent obsahuje sensitivity list pouze CLK a RESET a 
  --      - u kombinacnich komponent obsahuje sensitivity list vsechny ctene signaly. 


    pc: process (CLK, RESET) is
      begin
        if RESET = '1' then
          pc_out <= (others => '0');
        elsif rising_edge(CLK) then
          if pc_inc = '1' then
            pc_out <= pc_out + 1;
          elsif pc_dec = '1' then
            pc_out <= pc_out - 1;
          end if;
        end if;
    end process; 
      

    
    ptr: process (CLK, RESET) is
      begin
        if RESET = '1' then
          ptr_out <= (others => '0');
        elsif rising_edge(CLK) then
          if ptr_reset = '1' then
            ptr_out <= (others => '0');
          elsif ptr_inc = '1' then
            ptr_out <= ptr_out + 1;
          elsif ptr_dec = '1' then
            ptr_out <= ptr_out - 1;
          end if;
        end if;
    end process; 


    cnt: process (CLK, RESET) is
      begin
        if RESET = '1' then
          cnt_out <= (others => '0');
          --state <= s_reset;
          elsif rising_edge(CLK) then
          if cnt_inc = '1' then
            cnt_out <= cnt_out + 1;
          elsif pc_dec = '1' then
            cnt_out <= cnt_out - 1;
          end if;
        end if;
      end process; 


    mx1: process (CLK, RESET, mx1Sel) is
      begin
        if RESET = '1' then
          mx1Out <= (others => '0');
        elsif rising_edge(CLK) then
          if mx1Sel = '0' then
            mx1Out <= ptr_out;
          else
            mx1Out <= pc_out;
          end if;
        end if;
      end process;
      DATA_ADDR <= mx1Out;


      

    mx2: process (CLK, RESET, mx2Sel) is
      begin
        if RESET = '1' then
          mx2Out <= (others => '0');
        elsif rising_edge(CLK) then
          if mx2Sel = "00" then
            mx2Out <= IN_DATA;
          elsif mx2Sel = "01" then
            mx2Out <= DATA_RDATA - 1;
          elsif mx2Sel = "10" then
            mx2Out <= DATA_RDATA + 1;
          else
            mx2Out <= (others => '0');
          end if;
        end if;
      end process;
      DATA_WDATA <= mx2Out;




    state_logic: process (CLK, RESET) is
      begin
        if RESET = '1' then
          state <= s_start;
          --state <= s_reset;
        elsif rising_edge(CLK) then
          if EN = '1' then
            state <= nState;
          end if;
        end if;
      end process;



    fsm: process (state, OUT_BUSY, IN_VLD, DATA_RDATA) is
      begin
      
        -------- inicializace defaultnich hodnot signalu
        
        DATA_EN <= '1';
        IN_REQ <= '0';
        OUT_WE <= '0';
        DATA_RDWR <= '0';

        pc_inc <= '0';
        pc_dec <= '0';

        ptr_inc <= '0';
        ptr_dec <= '0';
        ptr_reset <= '0';

        cnt_inc <= '0';
        cnt_dec <= '0';

        mx1Sel <= '0';
        mx2Sel <= "11";

        DONE <= '0';



        case state is

          when s_start =>
            READY <= '0'; -- po resetu musi byt ready 0, dokud se nedodela init
            DATA_RDWR <= '0'; -- budu cist data
            case DATA_RDATA is
              when x"40" =>
                ptr_dec <= '1';
                READY <= '1';
                nState <= s_fetch;
                mx1Sel <= '1'; -- chci vzit dalsi prvek z adresy PC
              when others =>
                ptr_inc <= '1';
                nState <= s_start;
            end case;


          when s_fetch_before =>
            mx1Sel <= '1';
            nState <= s_fetch;


          when s_fetch =>
            mx1Sel <= '1'; -- chci vzit dalsi prvek z adresy PC
            DATA_RDWR <= '0'; -- chci ho cist a nic nezapisovat
            nState <= s_decode;
          

          when s_decode =>
            case DATA_RDATA is
              when x"3C" => -- znak <
                pc_inc <= '1';
                ptr_dec <= '1';
                mx1Sel <= '0';
                nState <= s_ptr_dec;

              when x"3E" => -- znak >
                pc_inc <= '1';
                ptr_inc <= '1';
                mx1Sel <= '0';
                nState <= s_ptr_inc;

              when x"2B" => -- znak +
                mx1Sel <= '0';
                nState <= s_val_inc;

              when x"2D" => -- znak -
                mx1Sel <= '0';
                nState <= s_val_dec;

              when x"5B" => -- znak [
                mx1Sel <= '0';
                nState <= s_while_start_before;
      
              when x"5D" => -- znak ]
                mx1Sel <= '0';
                nState <= s_while_end;

              when x"7E" => -- znak ~
                mx1Sel <= '0';
                nState <= s_break;

              when x"2E" => -- znak .
                mx1Sel <= '0';
                nState <= s_write_possible;

              when x"2C" => -- znak ,
                IN_REQ <= '1';
                nState <= s_get;

              when x"40" => -- znak @
                DONE <= '1';
                nState <= s_halt;

              when others =>
                pc_inc <= '1';
                mx1Sel <= '1'; -- chci vzit dalsi prvek z adresy PC
                nState <= s_fetch_before;

              end case;
          ------------- INKREMENTACE A DEKREMENTACE HODNOT -------------
          when s_val_inc =>
            DATA_EN <= '1'; -- pro jistotu
            DATA_RDWR <= '0'; -- nastavim rezim pro cteni (nechci zatim prepisovat hodnotu)
            nState <= s_val_inc_2; -- pri dalsim hodinovem signalu budu moct precist data


          when s_val_inc_2 =>
            mx2Sel <= "10"; -- vyberu variantu ktera pricte +1
            mx1Sel <= '0'; --chci to udelat na adrese ptr
            pc_inc <= '1'; -- po dodelani incrementu chci jit na dalsi znak
            nState <= s_val_inc_3;


          when s_val_inc_3 =>
            DATA_RDWR <= '1'; -- zapisu na adresu ptr novou hodnotu (+1)
            nState <= s_fetch; -- jdu zpet do fetch, abych se pripravil na cteni dalsiho znaku
            mx1Sel <= '1'; -- chci vzit dalsi prvek z adresy PC
        
        
          when s_val_dec =>
            DATA_EN <= '1';
            DATA_RDWR <= '0';
            nState <= s_val_dec_2;


          when s_val_dec_2 =>
            mx2Sel <= "01";
            mx1Sel <= '0';
            pc_inc <= '1'; -- po dodelani incrementu chci jit na dalsi znak
            nState <= s_val_dec_3;


          when s_val_dec_3 =>
            DATA_RDWR <= '1';
            mx1Sel <= '1'; -- chci vzit dalsi prvek z adresy PC
            nState <= s_fetch;

          ------------------- UKONCENI PROGRAMU -----------------------

          when s_halt =>
            DONE <= '1';
            nState <= s_halt;

          ------------------- INSTRUKCE PRO MOVE ----------------------

          when s_ptr_inc =>
            mx1Sel <= '1'; -- chci vzit dalsi prvek z adresy PC
            nState <= s_fetch;

          when s_ptr_dec =>
            mx1Sel <= '1'; -- chci vzit dalsi prvek z adresy PC
            nState <= s_fetch;

          ------------------- INSTRUKCE PRO VYPIS ZNAKU -----------------

          when s_write_possible =>
            if OUT_BUSY = '1' then
              nState <= s_write_possible;
            else
              pc_inc <= '1';
              nState <= s_write;
            end if;

          when s_write =>
            OUT_DATA <= DATA_RDATA;
            OUT_WE <= '1';
            mx1Sel <= '1'; -- chci vzit dalsi prvek z adresy PC
            nState <= s_fetch;


            ------------------ INSTRUKCE PRO NACTENI ZNAKU ------------------------

          when s_get =>
            IN_REQ <= '1';
            mx1Sel <= '0';
            mx2Sel <= "00";
            if IN_VLD = '0' then
              nState <= s_get;
            else
              pc_inc <= '1';
              nState <= s_get_2;
            end if;


          when s_get_2 =>
            DATA_RDWR <= '1';
            mx1Sel <= '1'; -- chci vzit dalsi prvek z adresy PC
            nState <= s_fetch;

          ------------------------ ZACATEK WHILE CYKLU -------------------

          when s_while_start_before =>
            pc_inc <= '1'; -- jdu na dalsi znak v PC -> prvni znak za zavorkou
            nState <= s_while_start;


          when s_while_start =>
            DATA_RDWR <= '0'; -- budu cist data (rezim cteni nastavuji defaultne, tudiz nemusi byt dalsi mezistav)
            mx1Sel <= '1'; -- budu potrebovat hodnotu na PTR (kontrola, jestli zde neni hodnota nula)
            nState <= s_while_start_2;


          when s_while_start_2 =>
            -- pripad, kdy nebude splnena podminka -> na adrese ptr bude 0
            if DATA_RDATA = x"00" then -- pokud je na adrese PTR hodnota 0 -> neni splnena podminka -> chci opustit while cyklus
              mx1Sel <= '1';
              nState <= s_while_go_exit_xx;
            else
              mx1Sel <= '1'; -- pokud je podminka splnena, vezmu dalsi znak z PC (ktery jsem v predchozim stavu inkrementoval) a budu kod dal zpracovavat
              nState <= s_fetch_before;
            end if;

        
        ------------------ VYSTOUPENI Z WHILE CYKLU ------------------------

          when s_while_go_exit_xx =>
            mx1Sel <= '1';
            pc_inc <= '1';
            nState <= s_while_go_exit;
            
          when s_while_go_exit =>
            if DATA_RDATA = x"5D" then -- pokud jsem nasel pravou zavorku
              --pc_inc <= '1'; -- posunu se o znak dal (znak za koncem while cyklu)
              mx1Sel <= '1'; -- chci vzit dalsi prvek z adresy PC a budu dal normalne zpracovavat kod
              nState <= s_fetch_before;
            else
              --pc_inc <= '1'; --pokud jsem uzaviraci zavorku nenasel, jdu na dalsi znak, dokud ji nenajdu
              mx1Sel <= '1';
              nState <= s_while_go_exit_xx;
            end if;

          ----------------- UZAVIRACI ZAVORKA WHILE CYKLU ---------------------

          when s_while_end => -- pokud jsem nasel koncovou zavorku pri normalnim pruchodu kodem
            --pc_dec <= '1'; -- jdu o znak zpet
            DATA_RDWR <= '0'; -- chci cist data z RDATA
            mx1Sel <= '0';
            nState <= s_while_end_2;


          when s_while_end_2 =>
            if DATA_RDATA /= x"00" then -- pokud je na adrese PTR hodnota 0 -> neni splnena podminka -> chci opustit while cyklus
              mx1Sel <= '1';                  
              nState <= s_while_end_5;   
            else
              mx1Sel <= '1'; -- pokud je podminka splnena, vezmu dalsi znak z PC (ktery jsem v predchozim stavu inkrementoval) a budu kod dal zpracovavat
              pc_inc <= '1';
              mx1Sel <= '1'; -- chci vzit dalsi prvek z adresy PC
              nState <= s_fetch_before;
            end if;


          when s_while_end_4 =>
            mx1Sel <= '1';
            nState <= s_while_end_5;
        
          when s_while_end_5 =>
              mx1Sel <= '1';
              nState <= s_while_end_3;

          when s_while_end_3 =>
            if DATA_RDATA = x"5B" then -- pokud jsem nasel oteviraci zavorku (zacatek dalsiho pruchodu while cyklem)
              mx1Sel <= '1'; -- chci vzit dalsi prvek z adresy PC
              nState <= s_fetch; -- pujdu do stavu while_start abych zkontroloval, jestli stale plati podminka
            else
              pc_dec <= '1'; -- jinak budu dekrementovat, dokud nenajdu oteviraci zavorku
              mx1Sel <= '1';
              nState <= s_while_end_4;
            end if;


          when s_break =>
            DATA_RDWR <= '0';
            pc_inc <= '1';
            nState <= s_while_go_exit;
          

          when others =>
            nState <= s_fetch;

          end case;       
      end process;
end behavioral;
