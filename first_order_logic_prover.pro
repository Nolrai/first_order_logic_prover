TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += C++11
SOURCES += main.cpp
LIBS += -lboost_unit_test_framework
HEADERS += \
    deduction_tree.hpp \
    first_order_logic.hpp \
    function.hpp \
    praser.hpp \
    predicate.hpp \
    proof_tree.hpp \
    set_inserter.hpp \
    term_generator.hpp \
    term.hpp \
    test.hpp

OTHER_FILES += \
    theorem_prover.pro.user \
    LICENSE \
    README.md
