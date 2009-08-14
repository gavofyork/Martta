include(../martta.prf)
include(Statements.pri)
SOURCES += .Statements-composed.cpp
HEADERS +=  ./ReturnStatement.h ./Referenced.h ./InScopeReferenced.h ./GlobalReferenced.h ./ArgumentReferenced.h ./LocalReferenced.h ./BinaryOperation.h ./AssignmentOperation.h ./SimpleBinaryOperation.h ./SubscriptOperation.h ./NewOperation.h ./DeleteOperation.h ./GenericMemberOperation.h ./MemberOperation.h ./LongMemberOperation.h ./SimpleUnaryOperation.h ./ReferenceOperation.h ./DereferenceOperation.h ./UnaryOperation.h ./Operation.h ./Invocation.h ./Evaluation.h ./Literal.h ./BoolLiteral.h ./StringLiteral.h ./IntegerLiteral.h ./FloatLiteral.h ./IfStatement.h ./Loop.h ./ForLoop.h ./WhileLoop.h ./BreakStatement.h  ./ReturnStatement.cpp ./Referenced.cpp ./InScopeReferenced.cpp ./GlobalReferenced.cpp ./ArgumentReferenced.cpp ./LocalReferenced.cpp ./BinaryOperation.cpp ./AssignmentOperation.cpp ./SimpleBinaryOperation.cpp ./SubscriptOperation.cpp ./NewOperation.cpp ./DeleteOperation.cpp ./GenericMemberOperation.cpp ./MemberOperation.cpp ./LongMemberOperation.cpp ./SimpleUnaryOperation.cpp ./ReferenceOperation.cpp ./DereferenceOperation.cpp ./UnaryOperation.cpp ./Operation.cpp ./Invocation.cpp ./Evaluation.cpp ./Literal.cpp ./BoolLiteral.cpp ./StringLiteral.cpp ./IntegerLiteral.cpp ./FloatLiteral.cpp ./IfStatement.cpp ./Loop.cpp ./ForLoop.cpp ./WhileLoop.cpp ./BreakStatement.cpp
SVGs.files = Support/in.svg Support/out.svg
SVGs.path = Support
INSTALLS += SVGs
SVG.commands = @echo "Copying SVG data..." && mkdir -p "$$DESTDIR/Data" && cp $$SVGs.files "$$DESTDIR/Data"
QMAKE_EXTRA_TARGETS += SVG
PRE_TARGETDEPS += SVG
