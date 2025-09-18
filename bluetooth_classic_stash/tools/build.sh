echo "Computing file sizes on riscv toolchain"
echo ""
make clean &>compilation.txt;make all arg1=/opt/riscv/bin/riscv64-unknown-linux-gnu-gcc arg2=/opt/riscv/bin/riscv64-unknown-linux-gnu-nm arg3=/opt/riscv/bin/riscv64-unknown-linux-gnu-objdump &>compilation.txt;
> filesizes.txt
for FILE in $(find ../btc/ -name *.o);
do
    echo "$FILE" >> filesizes.txt
    echo "$(size $FILE)" >> filesizes.txt
    echo " " >> filesizes.txt
done
echo "Compiling on native toolchain"
make clean;make all arg1=cc arg2=nm arg3=od;
