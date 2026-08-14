library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity Test_DisplayAlarmTop is
    port (
        CLOCK_50 : in  std_logic;
        HEX0, HEX1, HEX2, HEX3, HEX4, HEX5 : out std_logic_vector(6 downto 0)
    );
end entity;

architecture Behavioral of Test_DisplayAlarmTop is
    signal blink_1Hz : std_logic := '0';
    signal counter   : integer := 0;
begin
    -- Simple blink toggle @ ~1Hz from 50MHz clock
    process(CLOCK_50)
    begin
        if rising_edge(CLOCK_50) then
            if counter < 25_000_000 then
                counter <= counter + 1;
            else
                counter <= 0;
                blink_1Hz <= not blink_1Hz;
            end if;
        end if;
    end process;

    U_Display: entity work.DisplayDriver_Group10(Behavioral)
        port map(
            hr1          => "0000",  -- dummy
            hr0          => "0000",
            min1         => "0000",
            min0         => "0000",
            sec1         => "0000",
            sec0         => "0000",
            blink_digit  => "000",   -- irrelevant during alarm
            blink_on     => blink_1Hz,
            alarm_active => '1',     -- FORCED HIGH to test ALARM mode
            HEX5         => HEX5,
            HEX4         => HEX4,
            HEX3         => HEX3,
            HEX2         => HEX2,
            HEX1         => HEX1,
            HEX0         => HEX0
        );
end architecture;
