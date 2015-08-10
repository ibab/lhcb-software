*CMZ :  3.01/00 31/10/94  12.08.08  by  ZEUS Offline Group
*-- Author :
        Subroutine ZDSCRD(Card, Addr, Value, IValue)
* ==================================================
*
*      Subroutine unpacks the Character data card i/p in Card. The
*  format of Card is:
*
*     Card = '    Integer       Real'
*
*  The integer is returned in Addr and the real in Value.
*
        Implicit NONE
*
        Character*(*) Card
        Character*80  Fmt
*
        Integer I1, I2, ICFNBL, Addr, IOff, Index, ICDECI, ICLOCL,
     + Exp, Point, IStrt, ILen, Frac, IValue
*
        Real Value
*
        External ICFNBL, ICDECI, ICLOCL
*
        I1 = ICFNBL(Card, 1, 80)
*
        I2    = Index(Card(I1:80), ' ')
        IOff  = I1 + I2 - 1
        Point = ICLOCL('.', 1, Card, 1, 80)
*
*..  If Point.Eq.0 then this is an integer so:
        If (Point.Eq.0.Or.Point.GT.IOff) Then
            Addr   = ICDECI(Card, I1, 80)
            IValue = Addr
*
*..  Check to see if the first word was real:
        ElseIf (Point.LT.IOff) Then
            I1 = 1
            I2 = 1
        EndIf
*
        IOff = I1 + I2 - 1
        IStrt = ICFNBL(Card, IOff, 80)
*
*..  If IStrt is 0 then Only an integer word:
        If (IStrt.Eq.81) Then
            Value = FLOAT(IValue)
            Goto 8900
        EndIf
*
        Exp   = ICLOCL('E', 1, Card, IOff, 80)
        Point = ICLOCL('.', 1, Card, IOff, 80)
*
        If (Point.Eq.0.Or.Point.Eq.81) Then
            IValue = ICDECI(Card, IStrt, 80)
            Value = Float(IValue)
            Goto 8900
        EndIf
*
        If (Exp.Eq.0) Then
            Read (Card(IOff:80), '(E80.0)') Value
            IValue = Value
*
        Else
*
            ILen  = Index(Card(IStrt:80), ' ') - 1
            Frac  = Exp - Point - 1
            Write (Fmt, '(''(E'',I10,''.'',I10,'')'')') ILen, Frac
            Read (Card(IStrt:80),Fmt) Value
            IValue = Int(Value)
        EndIf
*
8900    Continue
*
        End
