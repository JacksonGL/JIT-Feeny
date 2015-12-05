package feeny.nodes;

import com.oracle.truffle.api.CompilerDirectives;
import com.oracle.truffle.api.RootCallTarget;
import com.oracle.truffle.api.Truffle;
import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.FrameSlot;
import com.oracle.truffle.api.frame.FrameSlotKind;
import com.oracle.truffle.api.frame.FrameSlotTypeException;
import com.oracle.truffle.api.frame.MaterializedFrame;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.Feeny;

public class CallNode extends RootNode {

    final FrameSlot slot;
    final String name;
    @Children final RootNode[] args;

    public CallNode(String name_, RootNode[] args_, FrameDescriptor frameDescriptor) {
        super(Feeny.class, null, frameDescriptor);
        name = name_;
        slot = Utils.getTopFrameDescriptor(frameDescriptor).findFrameSlot(name);
        args = args_;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        // System.err.println("Evaluating " + this.getClass().getName() + ":" + name);
        RootCallTarget target = null;
        try {
            target = (RootCallTarget) Utils.getTopFrame(frame).getObject(slot);
        } catch (FrameSlotTypeException e) {
            System.err.println("Function not found!\n");
        }
        Object[] runtime_args = new Object[args.length];
        for (int i = 0; i < args.length; i++) {
            runtime_args[i] = args[i].execute(frame);
        }
        return target.call(runtime_args);
    }
}
