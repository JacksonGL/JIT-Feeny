package feeny.nodes;

import com.oracle.truffle.api.RootCallTarget;
import com.oracle.truffle.api.Truffle;
import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.Feeny;

public class IntNode extends RootNode {
    final int value;

    public IntNode(int v, FrameDescriptor frameDescriptor) {
        super(Feeny.class, null, frameDescriptor);
        value = v;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        // System.err.println("Evaluating " + this.getClass().getName());
        return new Integer(value);
    }
}
