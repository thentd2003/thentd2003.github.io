library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity PreScale_Group10 is
    port (
        Clkin  : in std_logic;
        Clkout : out std_logic
    );
end PreScale_Group10;

architecture Behavioral of PreScale_Group10 is
    constant MAX_COUNT : unsigned(24 downto 0) := to_unsigned(24_999_999, 25); -- 50M / 2 - 1
    signal counter     : unsigned(24 downto 0) := (others => '0');
    signal clk_div     : std_logic := '0';
begin
    process(Clkin)
    begin
        if rising_edge(Clkin) then
            if counter = MAX_COUNT then
                counter <= (others => '0');
                clk_div <= not clk_div;
            else
                counter <= counter + 1;
            end if;
        end if;
    end process;

    Clkout <= clk_div;
end Behavioral;
