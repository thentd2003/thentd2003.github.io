library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity BuzzerTest_Top is
    Port (
        CLOCK_50 : in  STD_LOGIC;
        SW       : in  STD_LOGIC_VECTOR(9 downto 0); -- Use SW[0] to enable buzzer
        BUZZER   : out STD_LOGIC
    );
end BuzzerTest_Top;

architecture Behavioral of BuzzerTest_Top is
    constant CLOCK_FREQ : integer := 50000000; -- 50 MHz
    constant BUZZ_FREQ  : integer := 1000;     -- 1 kHz
    constant HALF_PERIOD_CYCLES : integer := CLOCK_FREQ / (2 * BUZZ_FREQ);

    signal counter    : integer range 0 to HALF_PERIOD_CYCLES := 0;
    signal buzz_state : STD_LOGIC := '0';
begin

    process(CLOCK_50)
    begin
        if rising_edge(CLOCK_50) then
            if SW(0) = '1' then  -- Buzzer active only when SW[0] is ON
                if counter = HALF_PERIOD_CYCLES then
                    counter <= 0;
                    buzz_state <= not buzz_state;
                else
                    counter <= counter + 1;
                end if;
            else
                buzz_state <= '0';
                counter <= 0;
            end if;
        end if;
    end process;

    BUZZER <= buzz_state;
end Behavioral;
