package feeny.reader;

import com.oracle.truffle.api.nodes.RootNode;

public class CallExp implements Exp {
    public final String name;
    public final Exp[] args;

    public CallExp(String aName, Exp[] aArgs) {
        name = aName;
        args = aArgs;
    }

    public String toString() {
        return name + "(" + Utils.commas(args) + ")";
    }

    public RootNode toTruffle() {
        throw new UnsupportedOperationException("Array is not supported yet");
    }
}
