package feeny.nodes;

import com.oracle.truffle.api.RootCallTarget;
import com.oracle.truffle.api.Truffle;
import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.FrameSlot;
import com.oracle.truffle.api.frame.FrameSlotTypeException;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.Feeny;

public class CallNode extends RootNode {
    RootCallTarget target;
    String name;
    @Children final RootNode[] arg_nodes;
    FrameSlot fnSlot;

    public CallNode(RootCallTarget t, FrameDescriptor frameDescriptor) {
        super(Feeny.class, null, frameDescriptor);
        target = t;
        arg_nodes = new RootNode[0];
    }

    public CallNode(String name, RootNode[] args_n, FrameDescriptor fd) {
        super(Feeny.class, null, fd);
        this.name = name;
        this.arg_nodes = args_n;
        this.fnSlot = fd.findFrameSlot(name);
    }

    @Override
    public Object execute(VirtualFrame frame) {
        System.out.println("call node");
        Object ret = null;
        try {
            ScopeFnNode fnNode = (ScopeFnNode) frame.getObject(fnSlot);
            Object[] arguments = new Object[arg_nodes.length];
            FrameDescriptor fd = fnNode.getFrameDescriptor();
            // something is wrong, here
            VirtualFrame calleeFrame = Truffle.getRuntime().createVirtualFrame(arguments, fd);
            String[] arg_names = fnNode.getArgs();
            for (int i = 0; i < arg_nodes.length; i++) {
                FrameSlot slot = fd.findFrameSlot(arg_names[i]);
                calleeFrame.setObject(slot, arg_nodes[i].execute(frame));
            }
            ret = fnNode.getBody().execute(calleeFrame);
        } catch (FrameSlotTypeException e) {
            System.out.println("Function " + name + " is not found in scope.");
            System.exit(0);
        }
        return ret;
    }
}
