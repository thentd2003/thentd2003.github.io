library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity tb_DigitalClockTop_Group10 is
end tb_DigitalClockTop_Group10;

architecture behavior of tb_DigitalClockTop_Group10 is

    -- Component declaration
    component DigitalClockTop_Group10
        port (
            CLOCK_50     : in  std_logic;
            KEY          : in  std_logic_vector(3 downto 0);
            SW           : in  std_logic_vector(1 downto 0);
            HEX0, HEX1, HEX2, HEX3, HEX4, HEX5 : out std_logic_vector(6 downto 0);
            LEDR         : out std_logic_vector(3 downto 0);
            BUZZER       : out std_logic
        );
    end component;

    -- Signals
    signal CLOCK_50 : std_logic := '0';
    signal KEY      : std_logic_vector(3 downto 0) := (others => '1');
    signal SW       : std_logic_vector(1 downto 0) := "00";

    signal HEX0, HEX1, HEX2, HEX3, HEX4, HEX5 : std_logic_vector(6 downto 0);
    signal LEDR : std_logic_vector(3 downto 0);
    signal BUZZER : std_logic;

begin

    -- Instantiate DUT
    uut: DigitalClockTop_Group10
        port map (
            CLOCK_50 => CLOCK_50,
            KEY      => KEY,
            SW       => SW,
            HEX0     => HEX0,
            HEX1     => HEX1,
            HEX2     => HEX2,
            HEX3     => HEX3,
            HEX4     => HEX4,
            HEX5     => HEX5,
            LEDR     => LEDR,
            BUZZER   => BUZZER
        );

    -- Generate 50 MHz Clock
    clk_process : process
    begin
        while now < 6 sec loop
            CLOCK_50 <= '0';
            wait for 10 ns;
            CLOCK_50 <= '1';
            wait for 10 ns;
        end loop;
        wait;
    end process;

    -- Stimulus
    stim_proc : process
    begin
        -- Initial state
        KEY <= "1111";       -- All buttons unpressed
        SW  <= "00";          -- Not in mode select, not showing clock

        wait for 200 ns;
        SW <= "01";           -- Turn on mode_enable
        wait for 100 ns;
        KEY(1) <= '0';        -- Confirm to enter Display mode
        wait for 100 ns;
        KEY(1) <= '1';

        wait for 200 ns;
        SW <= "11";           -- Now allow clock display to be shown
        wait;

    end process;

end behavior;
