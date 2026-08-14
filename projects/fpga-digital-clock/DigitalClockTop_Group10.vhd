-- File: DigitalClockTop_Group10.vhd
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity DigitalClockTop_Group10 is
    port (
        CLOCK_50 : in  std_logic;
        KEY      : in  std_logic_vector(3 downto 0);
        SW       : in  std_logic_vector(2 downto 0);
        HEX0     : out std_logic_vector(6 downto 0);
        HEX1     : out std_logic_vector(6 downto 0);
        HEX2     : out std_logic_vector(6 downto 0);
        HEX3     : out std_logic_vector(6 downto 0);
        HEX4     : out std_logic_vector(6 downto 0);
        HEX5     : out std_logic_vector(6 downto 0);
        LEDR     : out std_logic_vector(9 downto 0);
        BUZZER   : out std_logic
    );
end entity;

architecture Structural of DigitalClockTop_Group10 is

    component PreScale_Group10
        port( Clkin : in std_logic; Clkout : out std_logic );
    end component;

    component TimeCounter_Group10
        port(
            clk        : in  std_logic;
            reset      : in  std_logic;
            enable     : in  std_logic;
            load_time  : in  std_logic;
            new_sec0   : in  std_logic_vector(3 downto 0);
            new_sec1   : in  std_logic_vector(3 downto 0);
            new_min0   : in  std_logic_vector(3 downto 0);
            new_min1   : in  std_logic_vector(3 downto 0);
            new_hr0    : in  std_logic_vector(3 downto 0);
            new_hr1    : in  std_logic_vector(3 downto 0);
            sec0, sec1 : out std_logic_vector(3 downto 0);
            min0, min1 : out std_logic_vector(3 downto 0);
            hr0,  hr1  : out std_logic_vector(3 downto 0)
        );
    end component;

    component ModeFSM_Group10
        port(
            clk           : in  std_logic;
            reset         : in  std_logic;
            mode_enable   : in  std_logic;
            key_next      : in  std_logic;
            key_nav       : in  std_logic;
            key_digit     : in  std_logic;
            key_commit    : in  std_logic;
            mode_disp     : out std_logic;
            mode_sett     : out std_logic;
            mode_seta     : out std_logic;
            blink_digit   : out std_logic_vector(2 downto 0);
            blink_on      : out std_logic;
            mode_number   : out std_logic_vector(1 downto 0);
            commit_time   : out std_logic;
            commit_alarm  : out std_logic
        );
    end component;

    component TimeSetControl_Group10
        port(
            clk           : in  std_logic;
            reset         : in  std_logic;
            enable_sett   : in  std_logic;
            enable_seta   : in  std_logic;
            key_digit     : in  std_logic;
            key_next      : in  std_logic;
            key_commit    : in  std_logic;
            blink_digit   : in  std_logic_vector(2 downto 0);
            time_sec0     : out std_logic_vector(3 downto 0);
            time_sec1     : out std_logic_vector(3 downto 0);
            time_min0     : out std_logic_vector(3 downto 0);
            time_min1     : out std_logic_vector(3 downto 0);
            time_hr0      : out std_logic_vector(3 downto 0);
            time_hr1      : out std_logic_vector(3 downto 0);
            new_sec0      : out std_logic_vector(3 downto 0);
            new_sec1      : out std_logic_vector(3 downto 0);
            new_min0      : out std_logic_vector(3 downto 0);
            new_min1      : out std_logic_vector(3 downto 0);
            new_hr0       : out std_logic_vector(3 downto 0);
            new_hr1       : out std_logic_vector(3 downto 0)
        );
    end component;

    component AlarmCompare_Group10
        port(
            hr1_c, hr0_c    : in  std_logic_vector(3 downto 0);
            min1_c, min0_c  : in  std_logic_vector(3 downto 0);
            sec1_c, sec0_c  : in  std_logic_vector(3 downto 0);
            hr1_a, hr0_a    : in  std_logic_vector(3 downto 0);
            min1_a, min0_a  : in  std_logic_vector(3 downto 0);
            sec1_a, sec0_a  : in  std_logic_vector(3 downto 0);
            alarm_triggered : out std_logic
        );
    end component;
	 -- Intermediate display selection based on mode (Set vs Display)
	 
	 signal tmp_hr1, tmp_hr0, tmp_min1, tmp_min0, tmp_sec1, tmp_sec0 : std_logic_vector(3 downto 0);
	 signal alarm_triggered_now : std_logic;
    signal clk_1Hz, reset_sig : std_logic;
    signal key_next_sig, key_confirm_sig, key_digit_sig, key_commit_sig : std_logic;
    signal sec0, sec1, min0, min1, hr0, hr1 : std_logic_vector(3 downto 0);
    signal buf_sec0, buf_sec1, buf_min0, buf_min1, buf_hr0, buf_hr1 : std_logic_vector(3 downto 0);
    signal new_sec0, new_sec1, new_min0, new_min1, new_hr0, new_hr1 : std_logic_vector(3 downto 0);
    signal sec0_al, sec1_al, min0_al, min1_al, hr0_al, hr1_al : std_logic_vector(3 downto 0);
    signal disp_sec0, disp_sec1, disp_min0, disp_min1, disp_hr0, disp_hr1 : std_logic_vector(3 downto 0);
    signal mode_disp, mode_sett, mode_seta, blink_on : std_logic;
    signal blink_digit : std_logic_vector(2 downto 0);
    signal mode_number : std_logic_vector(1 downto 0);
    signal commit_time_50MHz, commit_alarm_50MHz : std_logic;
    signal commit_time_1Hz, commit_alarm_1Hz : std_logic := '0';
    signal time_req, alarm_req : std_logic := '0';
    signal alarm_flag, buzzer_on : std_logic := '0';

begin
    reset_sig       <= SW(1);
    key_next_sig    <= not KEY(0);
    key_confirm_sig <= not KEY(1);
    key_digit_sig   <= not KEY(2);
    key_commit_sig  <= not KEY(3);

    ClockDiv: entity work.PreScale_Group10(Behavioral)
        port map(Clkin => CLOCK_50, Clkout => clk_1Hz);

    ModeControl: entity work.ModeFSM_Group10(Behavioral)
        port map(
            clk           => CLOCK_50,
            reset         => reset_sig,
            mode_enable   => SW(0),
            key_next      => key_next_sig,
            key_nav       => key_confirm_sig,
            key_digit     => key_digit_sig,
            key_commit    => key_commit_sig,
            mode_disp     => mode_disp,
            mode_sett     => mode_sett,
            mode_seta     => mode_seta,
            blink_digit   => blink_digit,
            blink_on      => blink_on,
            mode_number   => mode_number,
            commit_time   => commit_time_50MHz,
            commit_alarm  => commit_alarm_50MHz
        );

SetLogic: entity work.TimeSetControl_Group10(Behavioral)
    port map(
        clk           => CLOCK_50,
        reset         => reset_sig,
        enable_sett   => mode_sett,
        enable_seta   => mode_seta,
        key_digit     => key_digit_sig,
        key_next      => key_next_sig,
        key_commit    => key_commit_sig,
        key_al_commit => key_commit_sig,
        blink_digit   => blink_digit,

        -- Alarm buffer outputs
        time_sec0     => buf_sec0,
        time_sec1     => buf_sec1,
        time_min0     => buf_min0,
        time_min1     => buf_min1,
        time_hr0      => buf_hr0,
        time_hr1      => buf_hr1,

        -- New time values for loading
        new_sec0      => new_sec0,
        new_sec1      => new_sec1,
        new_min0      => new_min0,
        new_min1      => new_min1,
        new_hr0       => new_hr0,
        new_hr1       => new_hr1
    );




    process(clk_1Hz, reset_sig)
    begin
        if reset_sig = '1' then
            sec0_al <= "1001";
            sec1_al <= "1001";
            min0_al <= "1001";
            min1_al <= "1001";
            hr0_al  <= "1001";
            hr1_al  <= "1001";
        elsif rising_edge(clk_1Hz) then
            if commit_alarm_1Hz = '1' then
                sec0_al <= buf_sec0;
                sec1_al <= buf_sec1;
                min0_al <= buf_min0;
                min1_al <= buf_min1;
                hr0_al  <= buf_hr0;
                hr1_al  <= buf_hr1;
            end if;
        end if;
    end process;

    process(clk_1Hz) begin
        if rising_edge(clk_1Hz) then
            commit_time_1Hz <= time_req;
            commit_alarm_1Hz <= alarm_req;
        end if;
    end process;

    process(CLOCK_50, reset_sig)
        variable ack1, ack2 : std_logic := '0';
    begin
        if reset_sig='1' then
            time_req <= '0'; ack1 := '0'; ack2 := '0';
        elsif rising_edge(CLOCK_50) then
            ack1 := commit_time_1Hz; ack2 := ack1;
            if commit_time_50MHz='1' and mode_sett='1' then
                time_req <= '1';
            elsif ack2='1' then
                time_req <= '0';
            end if;
        end if;
    end process;

    process(CLOCK_50, reset_sig)
        variable a1, a2 : std_logic := '0';
    begin
        if reset_sig='1' then
            alarm_req <= '0'; a1 := '0'; a2 := '0';
        elsif rising_edge(CLOCK_50) then
            a1 := commit_alarm_1Hz; a2 := a1;
            if commit_alarm_50MHz='1' and mode_seta='1' then
                alarm_req <= '1';
            elsif a2='1' then
                alarm_req <= '0';
            end if;
        end if;
    end process;

    ClockCounter: entity work.TimeCounter_Group10(Behavioral)
        port map(
            clk       => clk_1Hz,
            reset     => reset_sig,
            enable    => '1',
            load_time => commit_time_1Hz,
            new_sec0  => new_sec0,
            new_sec1  => new_sec1,
            new_min0  => new_min0,
            new_min1  => new_min1,
            new_hr0   => new_hr0,
            new_hr1   => new_hr1,
            sec0      => sec0,
            sec1      => sec1,
            min0      => min0,
            min1      => min1,
            hr0       => hr0,
            hr1       => hr1
        );



tmp_sec0 <= buf_sec0 when SW(0) = '1' else sec0;
tmp_sec1 <= buf_sec1 when SW(0) = '1' else sec1;
tmp_min0 <= buf_min0 when SW(0) = '1' else min0;
tmp_min1 <= buf_min1 when SW(0) = '1' else min1;
tmp_hr0  <= buf_hr0  when SW(0) = '1' else hr0;
tmp_hr1  <= buf_hr1  when SW(0) = '1' else hr1;

-- Final display signals with alarm masking
disp_sec0 <= (others => '0') when alarm_flag = '1' else tmp_sec0;
disp_sec1 <= (others => '0') when alarm_flag = '1' else tmp_sec1;
disp_min0 <= (others => '0') when alarm_flag = '1' else tmp_min0;
disp_min1 <= (others => '0') when alarm_flag = '1' else tmp_min1;
disp_hr0  <= (others => '0') when alarm_flag = '1' else tmp_hr0;
disp_hr1  <= (others => '0') when alarm_flag = '1' else tmp_hr1;


    DisplayOutput: entity work.DisplayDriver_Group10(Behavioral)
        port map(
            hr1 => disp_hr1,
            hr0 => disp_hr0,
            min1 => disp_min1,
            min0 => disp_min0,
            sec1 => disp_sec1,
            sec0 => disp_sec0,
            blink_digit => blink_digit,
            blink_on => blink_on,
            alarm_active => alarm_flag,
            HEX5 => HEX5, HEX4 => HEX4, HEX3 => HEX3,
            HEX2 => HEX2, HEX1 => HEX1, HEX0 => HEX0
        );


AlarmChecker: entity work.AlarmCompare_Group10(Behavioral)
    port map(
        hr1_c => hr1, hr0_c => hr0,
        min1_c => min1, min0_c => min0,
        sec1_c => sec1, sec0_c => sec0,
        hr1_a => hr1_al, hr0_a => hr0_al,
        min1_a => min1_al, min0_a => min0_al,
        sec1_a => sec1_al, sec0_a => sec0_al,
        alarm_triggered => alarm_triggered_now
    );


process(CLOCK_50, reset_sig)
begin
    if reset_sig = '1' then
        alarm_flag <= '0';
    elsif rising_edge(CLOCK_50) then
        if alarm_triggered_now = '1' then
            alarm_flag <= '1';
        elsif SW(2) = '1' then
            alarm_flag <= '0';
        end if;
    end if;
end process;

buzzer_on <= alarm_flag;



    BUZZER <= buzzer_on;

    LEDR(2 downto 0) <= "100" when mode_disp = '1' else
                        "010" when mode_sett = '1' else
                        "001";
    LEDR(7 downto 6) <= (others => '0');
    LEDR(4 downto 3) <= (others => '0');
    LEDR(5) <= buzzer_on;
    LEDR(9) <= commit_time_1Hz;
    LEDR(8) <= commit_alarm_1Hz;

end architecture;
