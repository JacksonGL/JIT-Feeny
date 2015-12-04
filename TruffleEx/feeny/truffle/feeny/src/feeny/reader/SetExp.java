package feeny.reader;

import com.oracle.truffle.api.nodes.RootNode;

public class SetExp implements Exp {
    public final String name;
    public final Exp exp;

    public SetExp(String aName, Exp aExp) {
        name = aName;
        exp = aExp;
    }

    public String toString() {
        return name + " = " + exp;
    }

    public RootNode toTruffle() {
        throw new UnsupportedOperationException("Array is not supported yet");
    }
}
