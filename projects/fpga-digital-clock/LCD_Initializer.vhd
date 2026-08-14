-- LCD_Initializer.vhd
-- Automatically initializes HD44780-compatible LCD (4-bit mode)
-- Use in tandem with your main FSM. Asserts 'init_done' when ready.

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity LCD_Initializer is
    Port (
        clk       : in  STD_LOGIC;
        rst       : in  STD_LOGIC;
        busy      : in  STD_LOGIC;
        ready     : out STD_LOGIC;
        send      : out STD_LOGIC;
        data_out  : out STD_LOGIC_VECTOR(7 downto 0);
        rs_out    : out STD_LOGIC
    );
end LCD_Initializer;

architecture Behavioral of LCD_Initializer is

    type state_type is (
        WAIT_15MS, CMD_30_1, CMD_30_2, CMD_30_3, CMD_20,
        FUNC_SET, DISP_ON, ENTRY_MODE, CLEAR, DONE, WAIT_BUSY
    );

    signal state      : state_type := WAIT_15MS;
    signal next_state : state_type;
    signal delay_cnt  : integer range 0 to 1_000_000 := 0;
    signal trigger    : STD_LOGIC := '0';

begin

    -- Timing delay state machine
    process(clk, rst)
    begin
        if rst = '1' then
            state <= WAIT_15MS;
            delay_cnt <= 0;
            trigger <= '0';
        elsif rising_edge(clk) then
            case state is
                when WAIT_15MS =>
                    if delay_cnt = 750_000 then  -- ~15 ms @ 50 MHz
                        delay_cnt <= 0;
                        state <= CMD_30_1;
                    else
                        delay_cnt <= delay_cnt + 1;
                    end if;

                when CMD_30_1 | CMD_30_2 | CMD_30_3 | CMD_20 | FUNC_SET | DISP_ON | ENTRY_MODE | CLEAR =>
                    if trigger = '0' and busy = '0' then
                        trigger <= '1';
                    elsif trigger = '1' and busy = '1' then
                        trigger <= '0';
                        state <= WAIT_BUSY;
                    end if;

                when WAIT_BUSY =>
                    if busy = '0' then
                        case next_state is
                            when CMD_30_1 => state <= CMD_30_2;
                            when CMD_30_2 => state <= CMD_30_3;
                            when CMD_30_3 => state <= CMD_20;
                            when CMD_20   => state <= FUNC_SET;
                            when FUNC_SET => state <= DISP_ON;
                            when DISP_ON  => state <= ENTRY_MODE;
                            when ENTRY_MODE => state <= CLEAR;
                            when CLEAR    => state <= DONE;
                            when others   => state <= DONE;
                        end case;
                    end if;

                when DONE => null;
            end case;
        end if;
    end process;

    -- Command output logic
    process(state)
    begin
        send     <= '0';
        rs_out   <= '0';
        data_out <= (others => '0');
        next_state <= state;

        case state is
            when CMD_30_1 | CMD_30_2 | CMD_30_3 =>
                data_out <= x"30"; send <= trigger;
                next_state <= state;

            when CMD_20 =>
                data_out <= x"20"; send <= trigger;
                next_state <= CMD_20;

            when FUNC_SET =>
                data_out <= x"28"; send <= trigger;
                next_state <= FUNC_SET;

            when DISP_ON =>
                data_out <= x"0C"; send <= trigger;
                next_state <= DISP_ON;

            when ENTRY_MODE =>
                data_out <= x"06"; send <= trigger;
                next_state <= ENTRY_MODE;

            when CLEAR =>
                data_out <= x"01"; send <= trigger;
                next_state <= CLEAR;

            when others => null;
        end case;
    end process;

    -- Initialization complete flag
    ready <= '1' when state = DONE else '0';

end Behavioral;
