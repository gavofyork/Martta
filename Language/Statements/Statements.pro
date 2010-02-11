include(../martta.prf)
include(Statements.pri)
HEADERS +=  ./ReturnStatement.h ./ReferencedValue.h ./InScopeReferencedValue.h ./GlobalReferencedValue.h ./ArgumentReferencedValue.h ./LocalReferencedValue.h ./BinaryOperation.h ./AssignmentOperation.h ./SimpleBinaryOperation.h ./SubscriptOperation.h ./NewOperation.h ./DeleteOperation.h ./GenericMemberOperation.h ./MemberOperation.h ./LongMemberOperation.h ./SimpleUnaryOperation.h ./ReferenceOperation.h ./DereferenceOperation.h ./UnaryOperation.h ./Operation.h ./Invocation.h ./Evaluation.h ./Literal.h ./BoolLiteral.h ./StringLiteral.h ./IntegerLiteral.h ./FloatLiteral.h ./IfStatement.h ./Loop.h ./ForLoop.h ./WhileLoop.h ./BreakStatement.h
unix:HEADERS +=  ./ReturnStatement.cpp ./ReferencedValue.cpp ./InScopeReferencedValue.cpp ./GlobalReferencedValue.cpp ./ArgumentReferencedValue.cpp ./LocalReferencedValue.cpp ./BinaryOperation.cpp ./AssignmentOperation.cpp ./SimpleBinaryOperation.cpp ./SubscriptOperation.cpp ./NewOperation.cpp ./DeleteOperation.cpp ./GenericMemberOperation.cpp ./MemberOperation.cpp ./LongMemberOperation.cpp ./SimpleUnaryOperation.cpp ./ReferenceOperation.cpp ./DereferenceOperation.cpp ./UnaryOperation.cpp ./Operation.cpp ./Invocation.cpp ./Evaluation.cpp ./Literal.cpp ./BoolLiteral.cpp ./StringLiteral.cpp ./IntegerLiteral.cpp ./FloatLiteral.cpp ./IfStatement.cpp ./Loop.cpp ./ForLoop.cpp ./WhileLoop.cpp ./BreakStatement.cpp
SOURCES += .Statements-composed.cpp
DEFINES += M_API_Statements=M_INAPI

INSTALL_DATA.files =  Support/in.svg Support/out.svg Support/GlobalReferenced.svg
INSTALL_DATA.path = $$PREFIX/share/martta/plugins/Data
INSTALLS += INSTALL_DATA
!win32:DATA.commands = @echo "Copying data..." && mkdir -p "$$DESTDIR/Data" && cp $$INSTALL_DATA.files "$$DESTDIR/Data"
win32:DATA.commands = @echo Copying data.. && md $$replace(DESTDIR, "/", "\\")\\Data\\TBD &&  copy Support\\in.svg $$replace(DESTDIR, "/", "\\")\\Data && copy Support\\out.svg $$replace(DESTDIR, "/", "\\")\\Data && copy Support\\GlobalReferenced.svg $$replace(DESTDIR, "/", "\\")\\Data && rd $$replace(DESTDIR, "/", "\\")\\Data\\TBD
QMAKE_EXTRA_TARGETS += DATA
PRE_TARGETDEPS += DATA
