# for c in 0 1 2 3 4 5; do sudo cpufreq-set -c $c -g performance; done
# pidof t8
# sudo perf stat -d -p 7717

echo "building test applications"
mkdir -p variants
for e in 0 1; do
    for c in "g++" "clang++-10"; do
        for f in t*.dsp; do
            for v in 8 16 32 64 128 512 1024; do
                CXX=$c faust2alsa -t 0 -lang ocpp -vec -vs $v -es $e $f;
                mv ${f%.dsp} variants/${f%.dsp}_vs${v}_es${e}_${c}
            done
            CXX=$c faust2alsa -t 0 -lang ocpp -scal -es $e $f;
            mv ${f%.dsp} variants/${f%.dsp}_scal_es${e}_${c}
        done
    done
done
