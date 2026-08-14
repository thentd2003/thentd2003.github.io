library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity tb_ModeFSM_Group10 is
end tb_ModeFSM_Group10;

architecture behavior of tb_ModeFSM_Group10 is

    component ModeFSM_Group10
        port (
            clk           : in std_logic;
            reset         : in std_logic;
            mode_enable   : in std_logic;
            key_next      : in std_logic;
            key_confirm   : in std_logic;
            key_digit     : in std_logic;
            key_cancel    : in std_logic;
            mode_disp     : out std_logic;
            mode_sett     : out std_logic;
            mode_seta     : out std_logic;
            blink_digit   : out std_logic_vector(2 downto 0);
            blink_on      : out std_logic;
            mode_number   : out std_logic_vector(1 downto 0)
        );
    end component;

    -- Signals
    signal clk         : std_logic := '0';
    signal reset       : std_logic := '0';
    signal mode_enable : std_logic := '0';
    signal key_next    : std_logic := '0';
    signal key_confirm : std_logic := '0';
    signal key_digit   : std_logic := '0';
    signal key_cancel  : std_logic := '0';

    signal mode_disp   : std_logic;
    signal mode_sett   : std_logic;
    signal mode_seta   : std_logic;
    signal blink_digit : std_logic_vector(2 downto 0);
    signal blink_on    : std_logic;
    signal mode_number : std_logic_vector(1 downto 0);

    constant clk_period : time := 20 ns;

begin

    -- Clock generation (50 MHz)
    clk_process : process
    begin
        for i in 0 to 2000 loop -- Stop after 2000 clock cycles (20 µs * 2000 = 40 µs)
            clk <= '0'; wait for clk_period / 2;
            clk <= '1'; wait for clk_period / 2;
        end loop;
        wait;
    end process;

    -- DUT instance
    uut: ModeFSM_Group10
        port map (
            clk           => clk,
            reset         => reset,
            mode_enable   => mode_enable,
            key_next      => key_next,
            key_confirm   => key_confirm,
            key_digit     => key_digit,
            key_cancel    => key_cancel,
            mode_disp     => mode_disp,
            mode_sett     => mode_sett,
            mode_seta     => mode_seta,
            blink_digit   => blink_digit,
            blink_on      => blink_on,
            mode_number   => mode_number
        );

    -- Stimulus
    stim_proc: process
    begin
        -- Reset
        reset <= '1';
        wait for 40 ns;
        reset <= '0';

        -- Enable mode selection
        mode_enable <= '1';
        wait for 50 ns;

        -- KEY[0] → mode 0 → 1
        key_next <= '1'; wait for 20 ns; key_next <= '0'; wait for 80 ns;

        -- KEY[0] → mode 1 → 2
        key_next <= '1'; wait for 20 ns; key_next <= '0'; wait for 80 ns;

        -- Confirm mode 2 (set alarm)
        key_confirm <= '1'; wait for 20 ns; key_confirm <= '0'; wait for 100 ns;

        -- KEY[2] → cycle digit
        key_digit <= '1'; wait for 20 ns; key_digit <= '0'; wait for 100 ns;
        key_digit <= '1'; wait for 20 ns; key_digit <= '0'; wait for 100 ns;

        -- Cancel
        key_cancel <= '1'; wait for 20 ns; key_cancel <= '0'; wait for 100 ns;

        -- Done
        wait;
    end process;

end behavior;
