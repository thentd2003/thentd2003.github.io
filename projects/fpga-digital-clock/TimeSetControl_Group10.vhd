-- File: TimeSetControl_Group10.vhd
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity TimeSetControl_Group10 is
    port (
        clk            : in  std_logic;
        reset          : in  std_logic;
        enable_sett    : in  std_logic;  -- SW(0) = time-set mode
        enable_seta    : in  std_logic;  -- SW(0) = alarm-set mode
        key_digit      : in  std_logic;  -- KEY[2]: advance blink_digit
        key_next       : in  std_logic;  -- KEY[0]: increment selected digit
        key_commit     : in  std_logic;  -- KEY[3]: commit time
        key_al_commit  : in  std_logic;  -- KEY[3]: commit alarm
        blink_digit    : in  std_logic_vector(2 downto 0);

        -- live edit buffer (for display)
        time_sec0      : out std_logic_vector(3 downto 0);
        time_sec1      : out std_logic_vector(3 downto 0);
        time_min0      : out std_logic_vector(3 downto 0);
        time_min1      : out std_logic_vector(3 downto 0);
        time_hr0       : out std_logic_vector(3 downto 0);
        time_hr1       : out std_logic_vector(3 downto 0);

        -- commit strobes (one‐cycle pulses at 50 MHz)
        commit_time    : out std_logic;
        commit_alarm   : out std_logic;

        -- committed values
        new_sec0       : out std_logic_vector(3 downto 0);
        new_sec1       : out std_logic_vector(3 downto 0);
        new_min0       : out std_logic_vector(3 downto 0);
        new_min1       : out std_logic_vector(3 downto 0);
        new_hr0        : out std_logic_vector(3 downto 0);
        new_hr1        : out std_logic_vector(3 downto 0)
    );
end entity;

architecture Behavioral of TimeSetControl_Group10 is

    type time_array is array(0 to 5) of unsigned(3 downto 0);
    signal buf       : time_array := (others => (others => '0'));
    signal latched   : time_array := (others => (others => '0'));

    -- edge-detect registers
    signal prev_next, prev_digit    : std_logic := '0';
    signal prev_commit, prev_al_commit : std_logic := '0';

    -- internal strobes
    signal strobe_time, strobe_alarm  : std_logic;

begin

    process(clk, reset)
    begin
        if reset = '1' then
            buf           <= (others => (others => '0'));
            latched       <= (others => (others => '0'));
            prev_next     <= '0';
            prev_digit    <= '0';
            prev_commit   <= '0';
            prev_al_commit<= '0';
            strobe_time   <= '0';
            strobe_alarm  <= '0';
        elsif rising_edge(clk) then
            -- sample previous
            prev_next      <= key_next;
            prev_digit     <= key_digit;
            prev_commit    <= key_commit;
            prev_al_commit <= key_al_commit;

            -- default strobes low
            strobe_time  <= '0';
            strobe_alarm <= '0';

            if enable_sett = '1' or enable_seta = '1' then
                -- digit increment
                if key_next = '1' and prev_next = '0' then
                    case blink_digit is
                        when "000" => buf(0) <= (buf(0) + 1) mod 10;  -- sec0
                        when "001" => buf(1) <= (buf(1) + 1) mod 6;   -- sec1
                        when "010" => buf(2) <= (buf(2) + 1) mod 10;  -- min0
                        when "011" => buf(3) <= (buf(3) + 1) mod 6;   -- min1
                        when "100" => buf(4) <= (buf(4) + 1) mod 10;  -- hr0
                        when "101" => buf(5) <= (buf(5) + 1) mod 3;   -- hr1
                        when others => null;
                    end case;
                end if;

                -- commit time
                if enable_sett = '1' and key_commit = '1' and prev_commit = '0' then
                    latched    <= buf;
                    strobe_time<= '1';
                end if;

                -- commit alarm
                if enable_seta = '1' and key_al_commit = '1' and prev_al_commit = '0' then
                    latched    <= buf;
                    strobe_alarm<= '1';
                end if;
            end if;
        end if;
    end process;

    -- drive outputs
    time_sec0    <= std_logic_vector(buf(0));
    time_sec1    <= std_logic_vector(buf(1));
    time_min0    <= std_logic_vector(buf(2));
    time_min1    <= std_logic_vector(buf(3));
    time_hr0     <= std_logic_vector(buf(4));
    time_hr1     <= std_logic_vector(buf(5));

    commit_time  <= strobe_time;
    commit_alarm <= strobe_alarm;

    new_sec0     <= std_logic_vector(latched(0));
    new_sec1     <= std_logic_vector(latched(1));
    new_min0     <= std_logic_vector(latched(2));
    new_min1     <= std_logic_vector(latched(3));
    new_hr0      <= std_logic_vector(latched(4));
    new_hr1      <= std_logic_vector(latched(5));

end architecture;
