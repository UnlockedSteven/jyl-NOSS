isEmpty(GOOGLETEST_DIR):GOOGLETEST_DIR=$$(GOOGLETEST_DIR)

isEmpty(GOOGLETEST_DIR) {
    GOOGLETEST_DIR = "D:/360MoveData/Users/HUAWEI/Desktop/lab4-蒋昀霖231220040/project/googletest"
    !isEmpty(GOOGLETEST_DIR) {
        warning("Using googletest src dir specified at Qt Creator wizard")
        message("set GOOGLETEST_DIR as environment variable or qmake variable to get rid of this message")
    }
}

!isEmpty(GOOGLETEST_DIR): {
    INCLUDEPATH *= "$$GOOGLETEST_DIR/include"

    LIBS *= -L"$$GOOGLETEST_DIR/lib" -lgtest -lgmock
} else {
    LIBS *= -lgtest -lgmock
}
