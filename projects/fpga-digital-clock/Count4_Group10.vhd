library ieee;
use ieee.std_logic_1164.all;

entity Count4_Group10 is
    port (
        D     : in  std_logic_vector(3 downto 0); 
        load  : in  std_logic;                    
        EN    : in  std_logic;                    
        clk   : in  std_logic;                    
        Q     : out std_logic_vector(3 downto 0)  
    );
end Count4_Group10;

architecture Behavioral of Count4_Group10 is
    signal count : std_logic_vector(3 downto 0) := (others => '0');
    signal sum   : std_logic_vector(3 downto 0);
    signal carry : std_logic_vector(3 downto 0);
begin
    -- Ripple-carry incrementer logic
    sum(0) <= count(0) xor EN;
    carry(0) <= count(0) and EN;

    sum(1) <= count(1) xor carry(0);
    carry(1) <= count(1) and carry(0);

    sum(2) <= count(2) xor carry(1);
    carry(2) <= count(2) and carry(1);

    sum(3) <= count(3) xor carry(2);
    carry(3) <= count(3) and carry(2);

    -- Sync process
    process(clk)
    begin
        if rising_edge(clk) then
            if load = '1' then
                count <= D;    -- load D
            else
                count <= sum;  -- increment
            end if;
        end if;
    end process;

    Q <= count;
end Behavioral;
