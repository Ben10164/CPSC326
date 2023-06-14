# !/bin/bash
make

echo ./mypl --check examples/static-1.mypl
./mypl --check examples/static-1.mypl
echo ./mypl --check examples/static-array.mypl
./mypl --check examples/static-array.mypl
echo ./mypl --check examples/static-cond.mypl
./mypl --check examples/static-cond.mypl
echo ./mypl --check examples/static-exp.mypl
./mypl --check examples/static-exp.mypl
echo ./mypl --check examples/static-for.mypl
./mypl --check examples/static-for.mypl
echo ./mypl --check examples/static-fun-1.mypl
./mypl --check examples/static-fun-1.mypl
echo ./mypl --check examples/static-fun-2.mypl
./mypl --check examples/static-fun-2.mypl
echo ./mypl --check examples/static-struct-arrays.mypl
./mypl --check examples/static-struct-arrays.mypl
echo ./mypl --check examples/static-struct.mypl
./mypl --check examples/static-struct.mypl
