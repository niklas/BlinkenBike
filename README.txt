== Prerequisites

Platformio, Ruby and some gems. Gemfile TODO

For simulation, install processing.

== Usage

Write code in src and lib, then run `platformio run --target upload`.

== Animations

You can put Effects in effects/ and Alpha Transitions into transitions/. They
will be precompiled into according implementions automatically.

There might be animations beginning with 'test_'. These will only be included when BIKE_ENV=test.

You can select a subset of animations by supplying environment vars EFFECTS and TRANSITIONS


    EFFECTS=sinchase,solid TRANSITIONS=fade platformio run --target upload
