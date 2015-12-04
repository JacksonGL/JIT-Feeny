package feeny.reader;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.nodes.SetExpNode;

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

    public RootNode toTruffle(FrameDescriptor fd) {
        return new SetExpNode(name, exp.toTruffle(fd), fd);
    }
}
