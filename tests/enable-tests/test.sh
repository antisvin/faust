# for c in 0 1 2 3 4 5; do sudo cpufreq-set -c $c -g performance; done
# pidof t8
# sudo perf stat -d -p 7717

echo "building test applications"
mkdir -p vec_es1 vec_es0 scal_es1 scal_es0
for f in t*.dsp; do
    faust2alsa -t 0 -lang ocpp -vec -es 1 $f; mv ${f%.dsp} vec_es1
    faust2alsa -t 0 -lang ocpp -vec -es 0 $f; mv ${f%.dsp} vec_es0
    faust2alsa -t 0 -lang ocpp -es 1 $f; mv ${f%.dsp} scal_es1
    faust2alsa -t 0 -lang ocpp -es 0 $f; mv ${f%.dsp} scal_es0
done