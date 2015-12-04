package feeny.nodes;

import java.util.Arrays;

import com.oracle.truffle.api.RootCallTarget;
import com.oracle.truffle.api.Truffle;
import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.Feeny;

public class ArrayNode extends RootNode {
    @Child RootNode init;
    @Child RootNode length;

    public ArrayNode(RootNode length, RootNode init, FrameDescriptor frameDescriptor) {
        super(Feeny.class, null, frameDescriptor);
        this.length = length;
        this.init = init;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        System.err.println("Evaluating " + this.getClass().getName());
        int length_i = (Integer) length.execute(frame);
        Object[] eval = new Object[length_i];
        Arrays.fill(eval, init.execute(frame));
        return eval;
    }
}
