tests = $( ls traces )

for name in $( ls traces ); do
    echo "Running test $name... " 
    ./mdriver -f "traces/$name"
    echo " "
done