makelist :: Integer -> [Integer]
makelist 0 = []
makelist x = x : (makelist (x-1))

main :: IO ()
main = print $ makelist 512
