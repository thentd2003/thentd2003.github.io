library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity BCDCount2_Group10 is
    port(
        clr, EN, clk       : in  std_logic;
        max0, max1         : in  std_logic_vector(3 downto 0);  -- NEW: max ones/tens
        BCD0, BCD1         : out std_logic_vector(3 downto 0)
    );
end BCDCount2_Group10;

architecture Behavioral of BCDCount2_Group10 is
    component Count4_Group10
        port(
            D    : in  std_logic_vector(3 downto 0);
            load : in  std_logic;
            EN   : in  std_logic;
            clk  : in  std_logic;
            Q    : out std_logic_vector(3 downto 0)
        );
    end component;

    signal Q0, Q1: std_logic_vector(3 downto 0);
    signal load0, load1, inc1: std_logic;
    signal Q0_is_max, Q1_is_max: std_logic;
begin
    -------------------------------------------------------------------------
    -- Detect if unit or tens digit reaches max value
    -------------------------------------------------------------------------
    Q0_is_max <= '1' when Q0 = max0 else '0';
    Q1_is_max <= '1' when Q1 = max1 else '0';

    -------------------------------------------------------------------------
    -- Unit digit logic: reset at max or clr, increment tens if max
    -------------------------------------------------------------------------
    load0 <= '1' when (Q0_is_max = '1' and EN = '1') or clr = '1' else '0';
    inc1  <= '1' when (Q0_is_max = '1' and EN = '1') else '0';

    -------------------------------------------------------------------------
    -- Tens digit logic: reset at max or clr
    -------------------------------------------------------------------------
    load1 <= '1' when (Q1_is_max = '1' and inc1 = '1') or clr = '1' else '0';

    -------------------------------------------------------------------------
    -- Instantiate Unit and Tens BCD counters
    -------------------------------------------------------------------------
    U0: Count4_Group10 port map(
        D    => (others => '0'),
        load => load0,
        EN   => EN,
        clk  => clk,
        Q    => Q0
    );

    U1: Count4_Group10 port map(
        D    => (others => '0'),
        load => load1,
        EN   => inc1,
        clk  => clk,
        Q    => Q1
    );

    -------------------------------------------------------------------------
    -- Outputs
    -------------------------------------------------------------------------
    BCD0 <= Q0;
    BCD1 <= Q1;

end Behavioral;
