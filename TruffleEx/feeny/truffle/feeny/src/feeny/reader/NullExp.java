package feeny.reader;

import com.oracle.truffle.api.nodes.RootNode;

public class NullExp implements Exp {
    public String toString() {
        return "null";
    }

    public RootNode toTruffle() {
        throw new UnsupportedOperationException("Array is not supported yet");
    }
}
