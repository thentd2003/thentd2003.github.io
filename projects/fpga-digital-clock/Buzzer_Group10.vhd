library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity Buzzer_Group10 is
    port (
        clk     : in std_logic;  -- CLOCK_50 (50MHz)
        enable  : in std_logic;  -- alarm_triggered
        buzzer  : out std_logic  -- output pin for peep
    );
end Buzzer_Group10;

architecture Behavioral of Buzzer_Group10 is
    signal count    : unsigned(15 downto 0) := (others => '0');
    signal tone     : std_logic := '0';
begin
    process(clk)
    begin
        if rising_edge(clk) then
            if enable = '1' then
                if count = 24999 then         -- 50 MHz / (2 × 1 kHz) = 25000
                    count <= (others => '0');
                    tone  <= not tone;
                else
                    count <= count + 1;
                end if;
            else
                count <= (others => '0');
                tone  <= '0';
            end if;
        end if;
    end process;

    buzzer <= tone;
end Behavioral;
