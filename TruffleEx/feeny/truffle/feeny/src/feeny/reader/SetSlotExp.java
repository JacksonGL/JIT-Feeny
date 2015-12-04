package feeny.reader;

import com.oracle.truffle.api.nodes.RootNode;

public class SetSlotExp implements Exp {
    public final String name;
    public final Exp exp;
    public final Exp value;

    public SetSlotExp(String aName, Exp aExp, Exp aValue) {
        name = aName;
        exp = aExp;
        value = aValue;
    }

    public String toString() {
        return exp + "." + name + " = " + value;
    }

    public RootNode toTruffle() {
        throw new UnsupportedOperationException("Array is not supported yet");
    }
}
