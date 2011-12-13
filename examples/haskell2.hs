data S = S Integer Double Char
  deriving (Show)

makelist :: Integer -> [S]
makelist 0 = []
makelist x = (S 1 1.0 'x') : (makelist (x-1))

main :: IO ()
main = print $ makelist 512
