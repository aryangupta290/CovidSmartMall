Overall objective

To build a Covid Smart Mall model, consisting of a smart entry and exit mechanism
which restricts the maximum number of people in the mall to, say 100 and a
temperature checker which denies entry to any person with body temperature
more than say 100℃ and smart sanitizer dispenser.

What are the inputs/physical quantities measured  and How are
we going to use the input?

Our project uses a DHT-11 temperature sensor that’ll be installed on the entrance
measuring the body temperature of the person wishing to enter the mall , and does
not open the door controlled by a servo motor in case of high body temperature.
We install a HC-SR501 PIR motion sensor on the entrance gate and exit gate for
maintaining the number of people in the mall , and adhering to the maximum
capacity allowed due to the covid restrictions.
HC-SR501 PIR motion sensor at the entry gate increments the person count as a
person enters the mall and the one at the exit decreases the person count as the
person leaves the mall , for simplicity we’re assuming that there’ll be only one gate
corresponding to entry and one for exit.
And a smart sanitizer dispenser that uses PIR motion sensor and servo motor to
dispense the sanitizing fluid.

Description of the output if any ?

We can keep a track of the number of people who visited the mall as this can be
used both by mall authorities as well as health authorities to determine how many
people have body temperature above a certain thresh-hold. We can also use the
data to keep track of the amount of sanitizer used.The data can be made available
to the general public so that they could plan their visit accordingly
