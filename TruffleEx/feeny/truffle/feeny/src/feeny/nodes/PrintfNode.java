package feeny.nodes;

import com.oracle.truffle.api.frame.FrameDescriptor;
import com.oracle.truffle.api.frame.VirtualFrame;
import com.oracle.truffle.api.nodes.RootNode;

import feeny.Feeny;

public class PrintfNode extends RootNode {
    @Children RootNode nodes[];
    String format;

    public PrintfNode(String format, RootNode nodes[], FrameDescriptor frameDescriptor) {
        super(Feeny.class, null, frameDescriptor);
        this.nodes = nodes;
        this.format = format;
    }

    @Override
    public Object execute(VirtualFrame frame) {
        int node_ind = 0;
        int str_index = 0;

        while (node_ind < nodes.length) {
            int end_index = format.indexOf('~', str_index);
            System.out.print(format.substring(str_index, end_index));
            str_index = end_index + 1;
            Object value = nodes[node_ind].execute(frame);
            System.out.print(value.toString()); // stand in
            node_ind++;
        }
        if (str_index != format.length()) {
            System.out.print(format.substring(str_index));
        }
        return null;
    }
}
