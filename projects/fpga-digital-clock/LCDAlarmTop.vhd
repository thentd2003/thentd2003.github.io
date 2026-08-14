-- LCDAlarmTop.vhd (Fixed: no 'when' in port map)

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity LCDAlarmTop is
    Port (
        CLOCK_50     : in  STD_LOGIC;
        SW           : in  STD_LOGIC_VECTOR(9 downto 0);
        LCD_RS_OUT   : out STD_LOGIC;
        LCD_RW_OUT   : out STD_LOGIC;
        LCD_E_OUT    : out STD_LOGIC;
        LCD_DATA_OUT : out STD_LOGIC_VECTOR(7 downto 4)
    );
end LCDAlarmTop;

architecture Behavioral of LCDAlarmTop is

    constant CLOCK_FREQ  : integer := 50000000;
    constant BLINK_FREQ  : integer := 2;
    constant HALF_PERIOD : integer := CLOCK_FREQ / (2 * BLINK_FREQ);

    signal counter        : integer range 0 to HALF_PERIOD := 0;
    signal blink_flag     : STD_LOGIC := '0';

    signal fsm_data       : STD_LOGIC_VECTOR(7 downto 0);
    signal fsm_rs         : STD_LOGIC;
    signal fsm_send       : STD_LOGIC := '0';
    signal lcd_busy       : STD_LOGIC;

    signal lcd_data_bus   : STD_LOGIC_VECTOR(7 downto 4);
    signal lcd_rs_sig     : STD_LOGIC;
    signal lcd_rw_sig     : STD_LOGIC;
    signal lcd_e_sig      : STD_LOGIC;

    signal init_data      : STD_LOGIC_VECTOR(7 downto 0);
    signal init_rs        : STD_LOGIC;
    signal init_send      : STD_LOGIC;
    signal init_done      : STD_LOGIC;

    signal lcd_data_mux   : STD_LOGIC_VECTOR(7 downto 0);
    signal lcd_rs_mux     : STD_LOGIC;
    signal lcd_send_mux   : STD_LOGIC;

    type state_type is (IDLE, SEND, WAIT_LCD);
    signal state : state_type := IDLE;

    component LCD_Controller
        Port (
            clk      : in  STD_LOGIC;
            rst      : in  STD_LOGIC;
            send     : in  STD_LOGIC;
            data_in  : in  STD_LOGIC_VECTOR(7 downto 0);
            rs_in    : in  STD_LOGIC;
            busy     : out STD_LOGIC;
            LCD_RS   : out STD_LOGIC;
            LCD_RW   : out STD_LOGIC;
            LCD_E    : out STD_LOGIC;
            LCD_DATA : out STD_LOGIC_VECTOR(7 downto 4)
        );
    end component;

    component LCD_Initializer
        Port (
            clk       : in  STD_LOGIC;
            rst       : in  STD_LOGIC;
            busy      : in  STD_LOGIC;
            ready     : out STD_LOGIC;
            send      : out STD_LOGIC;
            data_out  : out STD_LOGIC_VECTOR(7 downto 0);
            rs_out    : out STD_LOGIC
        );
    end component;

begin

    -- Blink timer
    process(CLOCK_50)
    begin
        if rising_edge(CLOCK_50) then
            if init_done = '1' and SW(0) = '1' then
                if counter = HALF_PERIOD then
                    counter    <= 0;
                    blink_flag <= not blink_flag;
                else
                    counter <= counter + 1;
                end if;
            else
                blink_flag <= '0';
                counter    <= 0;
            end if;
        end if;
    end process;

    -- FSM to blink LCD
    process(CLOCK_50)
    begin
        if rising_edge(CLOCK_50) then
            if init_done = '1' then
                case state is
                    when IDLE =>
                        fsm_send <= '0';
                        if SW(0) = '1' then
                            if blink_flag = '1' then
                                fsm_data <= x"08"; -- Display OFF
                            else
                                fsm_data <= x"0C"; -- Display ON
                            end if;
                            fsm_rs <= '0';
                            state  <= SEND;
                        end if;

                    when SEND =>
                        fsm_send <= '1';
                        state    <= WAIT_LCD;

                    when WAIT_LCD =>
                        fsm_send <= '0';
                        if lcd_busy = '0' then
                            state <= IDLE;
                        end if;
                end case;
            end if;
        end if;
    end process;

    -- Mux logic for LCD source
    lcd_data_mux <= init_data when init_done = '0' else fsm_data;
    lcd_rs_mux   <= init_rs   when init_done = '0' else fsm_rs;
    lcd_send_mux <= init_send when init_done = '0' else fsm_send;

    -- Controller
    lcd_ctrl: LCD_Controller
        port map (
            clk      => CLOCK_50,
            rst      => '0',
            send     => lcd_send_mux,
            data_in  => lcd_data_mux,
            rs_in    => lcd_rs_mux,
            busy     => lcd_busy,
            LCD_RS   => lcd_rs_sig,
            LCD_RW   => lcd_rw_sig,
            LCD_E    => lcd_e_sig,
            LCD_DATA => lcd_data_bus
        );

    lcd_init: LCD_Initializer
        port map (
            clk      => CLOCK_50,
            rst      => '0',
            busy     => lcd_busy,
            ready    => init_done,
            send     => init_send,
            data_out => init_data,
            rs_out   => init_rs
        );

    -- Drive LCD pins
    LCD_RS_OUT   <= lcd_rs_sig;
    LCD_RW_OUT   <= lcd_rw_sig;
    LCD_E_OUT    <= lcd_e_sig;
    LCD_DATA_OUT <= lcd_data_bus;

end Behavioral;
