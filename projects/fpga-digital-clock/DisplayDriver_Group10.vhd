-- File: DisplayDriver_Group10.vhd
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity DisplayDriver_Group10 is
    port (
        hr1, hr0     : in  std_logic_vector(3 downto 0);
        min1, min0   : in  std_logic_vector(3 downto 0);
        sec1, sec0   : in  std_logic_vector(3 downto 0);
        blink_digit  : in  std_logic_vector(2 downto 0);
        blink_on     : in  std_logic;
        alarm_active : in  std_logic;
        HEX5, HEX4, HEX3, HEX2, HEX1, HEX0 : out std_logic_vector(6 downto 0)
    );
end DisplayDriver_Group10;

architecture Behavioral of DisplayDriver_Group10 is
    -- BCD to 7-segment decoder
    function BCD_to_7seg(bcd : std_logic_vector(3 downto 0)) return std_logic_vector is
        variable seg : std_logic_vector(6 downto 0);
    begin
        case bcd is
            when "0000" => seg := "1000000"; -- 0
            when "0001" => seg := "1111001"; -- 1
            when "0010" => seg := "0100100"; -- 2
            when "0011" => seg := "0110000"; -- 3
            when "0100" => seg := "0011001"; -- 4
            when "0101" => seg := "0010010"; -- 5
            when "0110" => seg := "0000010"; -- 6
            when "0111" => seg := "1111000"; -- 7
            when "1000" => seg := "0000000"; -- 8
            when "1001" => seg := "0010000"; -- 9
            when others => seg := "1111111"; -- blank
        end case;
        return seg;
    end function;

    -- Custom letters for "ALARM"
    constant SEG_A : std_logic_vector(6 downto 0) := "0001000";

begin

process(blink_digit, blink_on, alarm_active, hr1, hr0, min1, min0, sec1, sec0)
begin
		if alarm_active = '1' then
				  -- Show "AAAAAA"
				  HEX5 <= SEG_A;
				  HEX4 <= SEG_A;
				  HEX3 <= SEG_A;
				  HEX2 <= SEG_A;
				  HEX1 <= SEG_A;
				  HEX0 <= SEG_A;

    else
        -- Normal clock display with blink
        if blink_on = '1' and blink_digit = "000" then
            HEX0 <= "1111111";
        else
            HEX0 <= BCD_to_7seg(sec0);
        end if;

        if blink_on = '1' and blink_digit = "001" then
            HEX1 <= "1111111";
        else
            HEX1 <= BCD_to_7seg(sec1);
        end if;

        if blink_on = '1' and blink_digit = "010" then
            HEX2 <= "1111111";
        else
            HEX2 <= BCD_to_7seg(min0);
        end if;

        if blink_on = '1' and blink_digit = "011" then
            HEX3 <= "1111111";
        else
            HEX3 <= BCD_to_7seg(min1);
        end if;

        if blink_on = '1' and blink_digit = "100" then
            HEX4 <= "1111111";
        else
            HEX4 <= BCD_to_7seg(hr0);
        end if;

        if blink_on = '1' and blink_digit = "101" then
            HEX5 <= "1111111";
        else
            HEX5 <= BCD_to_7seg(hr1);
        end if;
    end if;
end process;


end Behavioral;