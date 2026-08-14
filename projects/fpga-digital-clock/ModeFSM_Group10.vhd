library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity ModeFSM_Group10 is
    port (
        clk           : in  std_logic;                     -- 50 MHz clock
        reset         : in  std_logic;                     -- async reset (active high)
        mode_enable   : in  std_logic;                     -- SW(0): enables mode selection
        key_next      : in  std_logic;                     -- KEY(0): unused here
        key_nav       : in  std_logic;                     -- KEY(1): cycle through FSM modes
        key_digit     : in  std_logic;                     -- KEY(2): select which digit to edit
        key_commit    : in  std_logic;                     -- KEY(3): confirm (set) time or alarm
        mode_disp     : out std_logic;                     -- FSM in Display Time mode
        mode_sett     : out std_logic;                     -- FSM in Set Time mode
        mode_seta     : out std_logic;                     -- FSM in Set Alarm mode
        blink_digit   : out std_logic_vector(2 downto 0);  -- which digit should blink
        blink_on      : out std_logic;                     -- blinking enable (1 Hz)
        mode_number   : out std_logic_vector(1 downto 0);  -- binary mode encoding for LEDR
        commit_time   : out std_logic;                     -- 1-cycle pulse to commit time
        commit_alarm  : out std_logic                      -- 1-cycle pulse to commit alarm
    );
end entity;

architecture Behavioral of ModeFSM_Group10 is

    -- One-hot encoding: Only one of these three is '1' at any time
    signal st_display  : std_logic := '1';  -- Initial state (default)
    signal st_settime  : std_logic := '0';
    signal st_setalarm : std_logic := '0';

    -- Debounced key tracking (rising edge detection)
    signal prev_nav, prev_commit, prev_digit, prev_next : std_logic := '0';

    -- Blink digit selector: ranges from 000 to 101 (HEX0 to HEX5)
    signal digit_idx  : unsigned(2 downto 0) := (others => '0');

    -- Blinker: 1 Hz toggle from 50 MHz clock
    signal blink_cnt  : unsigned(24 downto 0) := (others => '0');
    signal blink_reg  : std_logic := '0';

    -- Output pulses for confirming time/alarm set
    signal commit_time_sig, commit_alarm_sig : std_logic := '0';

begin

    -- MAIN FSM: One-hot encoding logic with rising-edge key detection 
    process(clk, reset)
    begin
        if reset = '1' then
            -- On reset: go to Display mode
            st_display      <= '1';
            st_settime      <= '0';
            st_setalarm     <= '0';

            -- Clear all internal registers
            digit_idx       <= (others => '0');
            prev_nav        <= '0';
            prev_commit     <= '0';
            prev_digit      <= '0';
            prev_next       <= '0';
            commit_time_sig <= '0';
            commit_alarm_sig<= '0';

        elsif rising_edge(clk) then
            -- Capture previous values to detect key rising edges
            prev_nav    <= key_nav;
            prev_commit <= key_commit;
            prev_digit  <= key_digit;
            prev_next   <= key_next;

            -- Clear pulses every cycle unless triggered
            commit_time_sig  <= '0';
            commit_alarm_sig <= '0';

            -- FSM is frozen unless mode_enable = '1' (SW(0) toggle)
            if mode_enable = '0' then
                -- Return to Display mode (safe fallback)
                st_display  <= '1';
                st_settime  <= '0';
                st_setalarm <= '0';
                digit_idx   <= (others => '0');

            else
                -- FSM transitions (only one state is '1' at a time)

                if st_display = '1' then
                    -- Move from Display → Set Time
                    if key_nav = '1' and prev_nav = '0' then
                        st_display  <= '0';
                        st_settime  <= '1';
                        st_setalarm <= '0';
                    end if;

                elsif st_settime = '1' then
                    -- Set Time → Set Alarm
                    if key_nav = '1' and prev_nav = '0' then
                        st_display  <= '0';
                        st_settime  <= '0';
                        st_setalarm <= '1';

                    -- Digit selection (cycles from 0 to 5)
                    elsif key_digit = '1' and prev_digit = '0' then
                        if digit_idx = "101" then
                            digit_idx <= "000";
                        else
                            digit_idx <= digit_idx + 1;
                        end if;

                    -- Commit time to TimeCounter
                    elsif key_commit = '1' and prev_commit = '0' then
                        commit_time_sig <= '1';
                    end if;

                elsif st_setalarm = '1' then
                    -- Set Alarm → Display
                    if key_nav = '1' and prev_nav = '0' then
                        st_display  <= '1';
                        st_settime  <= '0';
                        st_setalarm <= '0';

                    -- Digit selection (cycles from 0 to 5)
                    elsif key_digit = '1' and prev_digit = '0' then
                        if digit_idx = "101" then
                            digit_idx <= "000";
                        else
                            digit_idx <= digit_idx + 1;
                        end if;

                    -- Commit alarm to alarm registers
                    elsif key_commit = '1' and prev_commit = '0' then
                        commit_alarm_sig <= '1';
                    end if;
                end if;
            end if;
        end if;
    end process;

    -- Output Pulse Control
    process(clk)
    begin
        if rising_edge(clk) then
            -- One-cycle output pulses
            commit_time  <= commit_time_sig;
            commit_alarm <= commit_alarm_sig;
        end if;
    end process;

    -- Blink Register Generation (1Hz)
    process(clk)
    begin
        if rising_edge(clk) then
            if blink_cnt = 25_000_000 - 1 then  -- toggle every 1 second
                blink_cnt <= (others => '0');
                blink_reg <= not blink_reg;
            else
                blink_cnt <= blink_cnt + 1;
            end if;
        end if;
    end process;

    -- Output Signals Mapping 

    mode_disp   <= st_display;                                -- HEX display shows time
    mode_sett   <= st_settime;                                -- Set Time mode active
    mode_seta   <= st_setalarm;                               -- Set Alarm mode active
    blink_on    <= blink_reg when (st_settime = '1' or st_setalarm = '1') else '0';
    blink_digit <= std_logic_vector(digit_idx);               -- current blinking digit

    -- Useful for LEDR(2:0)
    mode_number <= "00" when st_display  = '1' else
                   "01" when st_settime  = '1' else
                   "10"; -- only one-hot active, so "10" = Set Alarm

end architecture;
