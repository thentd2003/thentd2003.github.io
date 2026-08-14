library ieee;
use ieee.std_logic_1164.all;

entity AlarmCompare_Group10 is
    port (
        -- Current time from TimeCounter
        hr1_c, hr0_c : in std_logic_vector(3 downto 0);
        min1_c, min0_c : in std_logic_vector(3 downto 0);
        sec1_c, sec0_c : in std_logic_vector(3 downto 0);

        -- Alarm time to compare
        hr1_a, hr0_a : in std_logic_vector(3 downto 0);
        min1_a, min0_a : in std_logic_vector(3 downto 0);
        sec1_a, sec0_a : in std_logic_vector(3 downto 0);

        -- Output: goes high when time matches alarm
        alarm_triggered : out std_logic
    );
end AlarmCompare_Group10;

architecture Behavioral of AlarmCompare_Group10 is
begin
    process(hr1_c, hr0_c, min1_c, min0_c, sec1_c, sec0_c,
            hr1_a, hr0_a, min1_a, min0_a, sec1_a, sec0_a)
    begin
        -- Only trigger alarm if alarm time is NOT 99:99:99
        if not (
            hr1_a = "1001" and hr0_a = "1001" and
            min1_a = "1001" and min0_a = "1001" and
            sec1_a = "1001" and sec0_a = "1001"
        ) then
            if (hr1_c = hr1_a and hr0_c = hr0_a and
                min1_c = min1_a and min0_c = min0_a and
                sec1_c = sec1_a and sec0_c = sec0_a) then
                alarm_triggered <= '1';
            else
                alarm_triggered <= '0';
            end if;
        else
            alarm_triggered <= '0';  -- Alarm time is invalid: don't trigger
        end if;
    end process;
end Behavioral;
