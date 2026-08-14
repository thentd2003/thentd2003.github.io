-- File: TimeCounter_Group10.vhd
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity TimeCounter_Group10 is
    port (
        clk        : in  std_logic;
        reset      : in  std_logic;
        enable     : in  std_logic;
        load_time  : in  std_logic;
        new_sec0, new_sec1 : in  std_logic_vector(3 downto 0);
        new_min0, new_min1 : in  std_logic_vector(3 downto 0);
        new_hr0,  new_hr1  : in  std_logic_vector(3 downto 0);
        sec0, sec1 : out std_logic_vector(3 downto 0);
        min0, min1 : out std_logic_vector(3 downto 0);
        hr0,  hr1  : out std_logic_vector(3 downto 0)
    );
end TimeCounter_Group10;

architecture Behavioral of TimeCounter_Group10 is

    signal sec_ones, sec_tens : std_logic_vector(3 downto 0) := (others => '0');
    signal min_ones, min_tens : std_logic_vector(3 downto 0) := (others => '0');
    signal hr_ones,  hr_tens  : std_logic_vector(3 downto 0) := (others => '0');

begin
    process(clk, reset)
    begin
        if reset = '1' then
            sec_ones <= (others => '0');
            sec_tens <= (others => '0');
            min_ones <= (others => '0');
            min_tens <= (others => '0');
            hr_ones  <= (others => '0');
            hr_tens  <= (others => '0');

        elsif rising_edge(clk) then

            if load_time = '1' then
                -- load new time/alarm
                sec_ones <= new_sec0;
                sec_tens <= new_sec1;
                min_ones <= new_min0;
                min_tens <= new_min1;
                hr_ones  <= new_hr0;
                hr_tens  <= new_hr1;

            elsif enable = '1' then
                -- seconds
                if sec_ones = "1001" then
                    sec_ones <= "0000";
                    if sec_tens = "0101" then
                        sec_tens <= "0000";
                        -- minutes
                        if min_ones = "1001" then
                            min_ones <= "0000";
                            if min_tens = "0101" then
                                min_tens <= "0000";
                                -- hours with correct 24-hour wrap
                                if hr_tens = "0010" and hr_ones = "0011" then
                                    -- at 23 → wrap to 00
                                    hr_tens <= "0000";
                                    hr_ones <= "0000";
                                elsif hr_ones = "1001" then
                                    -- at x9 → increment tens (0→1 or 1→2)
                                    hr_ones <= "0000";
                                    hr_tens <= std_logic_vector(unsigned(hr_tens) + 1);
                                else
                                    -- normal ones increment
                                    hr_ones <= std_logic_vector(unsigned(hr_ones) + 1);
                                end if;
                            else
                                -- increment minute-tens
                                min_tens <= std_logic_vector(unsigned(min_tens) + 1);
                            end if;
                        else
                            -- increment minute-ones
                            min_ones <= std_logic_vector(unsigned(min_ones) + 1);
                        end if;
                    else
                        -- increment second-tens
                        sec_tens <= std_logic_vector(unsigned(sec_tens) + 1);
                    end if;
                else
                    -- increment second-ones
                    sec_ones <= std_logic_vector(unsigned(sec_ones) + 1);
                end if;
            end if;
        end if;
    end process;

    -- drive outputs
    sec0 <= sec_ones;
    sec1 <= sec_tens;
    min0 <= min_ones;
    min1 <= min_tens;
    hr0  <= hr_ones;
    hr1  <= hr_tens;

end Behavioral;
