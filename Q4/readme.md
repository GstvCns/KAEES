Imagine that you have a microcontroller that communicates to a generic system that may consist
of several other boards via UART. How do you ensure that each message is properly sent and
that its content is correct?

    Assuming there isn't any other hardware connection (like a chip selection pin, bidirectional switches etc) connecting both boards,
    it can be managed using interboards protocols. The basic structure would be every board having an unique ID, or a functional one,
    and every message written by the 'master' board addresses one among the 'expansions' boards. A data section, being variable in size or not.
    Using preables is very recommended as a message validation method, commands to perform different actions,
    total message size and postambles are also useful in identifying the full message.

    The therms 'master' and 'expansions' are being used due to the electrical limitations of having only one UART's TX pin per bus,
    having a second one would lead into tristates values, which are not accepted at the logic level. However, there are no restrictions in
    the amount of UART's RX pins per bus. Concluding in a, kind of, half duplex connection between these boards, where there is a 'master'
    which will command an 'expansion' board to put its TX into the bus to respond.