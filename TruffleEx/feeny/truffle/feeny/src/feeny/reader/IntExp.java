package feeny.reader;

import com.oracle.truffle.api.nodes.RootNode;

public class IntExp implements Exp {
    public final int value;

    public IntExp(int aValue) {
        value = aValue;
    }

    public String toString() {
        return Integer.toString(value);
    }

    public RootNode toTruffle() {
        throw new UnsupportedOperationException("Array is not supported yet");
    }
}
