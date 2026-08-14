-- File: tb_AlarmDisplayTop.vhd (top-level simulation wrapper for alarm display logic)
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity tb_AlarmDisplayTop is
end tb_AlarmDisplayTop;

architecture sim of tb_AlarmDisplayTop is

    signal clk           : std_logic := '0';
    signal blink_on      : std_logic := '0';
    signal alarm_active  : std_logic := '0';
    signal blink_digit   : std_logic_vector(2 downto 0) := "000";

    signal hr1, hr0      : std_logic_vector(3 downto 0) := "0000";
    signal min1, min0    : std_logic_vector(3 downto 0) := "0000";
    signal sec1, sec0    : std_logic_vector(3 downto 0) := "0000";

    signal HEX0, HEX1, HEX2, HEX3, HEX4, HEX5 : std_logic_vector(6 downto 0);

    constant clk_period : time := 20 ns; -- 50 MHz clock

begin

    -- Clock generation
    clk_process : process
    begin
        while now < 2 ms loop
            clk <= '0'; wait for clk_period / 2;
            clk <= '1'; wait for clk_period / 2;
        end loop;
        wait;
    end process;

    -- Blink toggle (1Hz equivalent simulation)
    blink_gen : process
    begin
        wait for 500 ms;
        blink_on <= not blink_on;
    end process;

    -- Simulate alarm activation
    stimulus_proc: process
    begin
        wait for 100 ms;
        alarm_active <= '1';

        wait for 2 sec;
        alarm_active <= '0';

        wait;
    end process;

    -- Unit Under Test
    uut: entity work.DisplayDriver_Group10
        port map (
            hr1          => hr1,
            hr0          => hr0,
            min1         => min1,
            min0         => min0,
            sec1         => sec1,
            sec0         => sec0,
            blink_digit  => blink_digit,
            blink_on     => blink_on,
            alarm_active => alarm_active,
            HEX0 => HEX0,
            HEX1 => HEX1,
            HEX2 => HEX2,
            HEX3 => HEX3,
            HEX4 => HEX4,
            HEX5 => HEX5
        );

end sim;
