-- LCD_Controller.vhd
-- Works with HD44780-compatible 16x2 LCD on DE10-Standard
-- 4-bit mode controller for sending commands and characters

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity LCD_Controller is
    Port (
        clk        : in  STD_LOGIC; -- 50 MHz clock
        rst        : in  STD_LOGIC;
        send       : in  STD_LOGIC; -- Trigger to send a byte
        data_in    : in  STD_LOGIC_VECTOR(7 downto 0);
        rs_in      : in  STD_LOGIC; -- '0' = command, '1' = data
        busy       : out STD_LOGIC;

        LCD_RS     : out STD_LOGIC;
        LCD_RW     : out STD_LOGIC;
        LCD_E      : out STD_LOGIC;
        LCD_DATA   : out STD_LOGIC_VECTOR(7 downto 4)
    );
end LCD_Controller;

architecture Behavioral of LCD_Controller is

    type state_type is (IDLE, SEND_HIGH, WAIT_HIGH, SEND_LOW, WAIT_LOW, DONE);
    signal state     : state_type := IDLE;
    signal cnt       : integer range 0 to 255 := 0;
    signal byte_reg  : STD_LOGIC_VECTOR(7 downto 0) := (others => '0');
    signal rs_reg    : STD_LOGIC := '0';
    signal en        : STD_LOGIC := '0';

begin

    LCD_RW <= '0'; -- always write
    LCD_E  <= en;
    LCD_RS <= rs_reg;
    LCD_DATA <= byte_reg(7 downto 4) when state = SEND_HIGH or state = WAIT_HIGH else
                byte_reg(3 downto 0);

    busy <= '1' when state /= IDLE and state /= DONE else '0';

    process(clk, rst)
    begin
        if rst = '1' then
            state <= IDLE;
            en    <= '0';
            cnt   <= 0;
        elsif rising_edge(clk) then
            case state is
                when IDLE =>
                    if send = '1' then
                        byte_reg <= data_in;
                        rs_reg   <= rs_in;
                        state    <= SEND_HIGH;
                    end if;

                when SEND_HIGH =>
                    en <= '1';
                    state <= WAIT_HIGH;
                    cnt <= 0;

                when WAIT_HIGH =>
                    if cnt = 10 then
                        en <= '0';
                        state <= SEND_LOW;
                    else
                        cnt <= cnt + 1;
                    end if;

                when SEND_LOW =>
                    en <= '1';
                    state <= WAIT_LOW;
                    cnt <= 0;

                when WAIT_LOW =>
                    if cnt = 10 then
                        en <= '0';
                        state <= DONE;
                    else
                        cnt <= cnt + 1;
                    end if;

                when DONE =>
                    state <= IDLE;

            end case;
        end if;
    end process;

end Behavioral;
