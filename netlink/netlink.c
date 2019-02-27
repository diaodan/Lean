#include "head.h"

void nl_print_mac(void *mac)
{
    unsigned char *p = (unsigned char *)mac;
    printf("%.2x:%.2x:%.2x:%.2x:%.2x:%.2x ",
            p[0], p[1], p[2],
            p[3], p[4], p[5]);
}

void nl_print_qdisc(void *array, int len)
{
    char *p = (char *)array;
    while (len) {
        printf("%u ", *p);
        p ++;
        len --;
    }
}

void nl_print_link_stats(struct rtnl_link_stats *s)
{
    printf("rx_packets %u ", s->rx_packets);
    printf("tx_packets %u ", s->tx_packets);
    printf("rx_bytes %u ", s->rx_bytes);
    printf("tx_bytes %u ", s->tx_bytes);
    printf("rx_errors %u ", s->rx_errors);
    printf("tx_errors %u ", s->tx_errors);
    printf("rx_dropped %u ", s->rx_dropped);
    printf("tx_dropped %u ", s->tx_dropped);
}

void nl_print_link_map(struct rtnl_link_ifmap *m)
{
    printf("mem start %lu ", m->mem_start);
    printf("mem end %lu ", m->mem_end);
    printf("base addr %lu ", m->base_addr);
    printf("irq %u ", m->irq);
    printf("dma %u ", m->dma);
    printf("port %u ", m->port);
}

static void nl_parse_vlan_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_veth_info(struct rtattr *rta, unsigned int len)
{
    struct rtattr *vrta = NULL;
    struct rtattr *irta = NULL;
    struct ifinfomsg *peer = NULL;
    char name[32];

    while (RTA_OK(rta, len)) {
        if (rta->rta_type == VETH_INFO_PEER) {
            peer = (struct ifinfomsg *)RTA_DATA(rta);
            vrta = (struct rtattr *)((char *)peer + sizeof(struct ifinfomsg));
            if (vrta->rta_type == IFLA_IFNAME) {
                snprintf(name, 32, "%s", RTA_DATA(vrta));
                printf("%s ", name);
            }
            printf("index %u ", peer->ifi_index);
        }
        rta = RTA_NEXT(rta, len);
    }
}

static void nl_parse_vcan_info(struct rtattr *rta, unsigned int len)
{

}

static void nl_parse_dummy_info(struct rtattr *rta, unsigned int len)
{

}

static void nl_parse_ifb_info(struct rtattr *rta, unsigned int len)
{

}

static void nl_parse_macvlan_info(struct rtattr *rta, unsigned int len)
{

}

static void nl_parse_macvtap_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_bridge_info(struct rtattr *rta, unsigned int len)
{
    struct rtattr *vrta = NULL;
    struct rtattr *irta = NULL;
    unsigned int ipayload = 0;

    char name[32];

    printf("bridge link info ");

    while (RTA_OK(rta, len)) {
        switch (rta->rta_type) {
            case IFLA_BR_UNSPEC:
                printf("IFLA_BR_UNSPEC ");
                break;
            case IFLA_BR_FORWARD_DELAY:
                printf("IFLA_BR_FORWARD_DELAY ");
                break;
            case IFLA_BR_HELLO_TIME:
                printf("IFLA_BR_HELLO_TIME ");
                break;
            case IFLA_BR_MAX_AGE:
                printf("IFLA_BR_MAX_AGE ");
                break;
            case IFLA_BR_AGEING_TIME:
                printf("IFLA_BR_AGEING_TIME ");
                break;
            case IFLA_BR_STP_STATE:
                printf("IFLA_BR_STP_STATE ");
                break;
            case IFLA_BR_PRIORITY:
                printf("IFLA_BR_PRIORITY ");
                break;
            case IFLA_BR_VLAN_FILTERING:
                printf("IFLA_BR_VLAN_FILTERING ");
                break;
            case IFLA_BR_VLAN_PROTOCOL:
                printf("IFLA_BR_VLAN_PROTOCOL ");
                break;
            case IFLA_BR_GROUP_FWD_MASK:
                printf("IFLA_BR_GROUP_FWD_MASK ");
                break;
            case IFLA_BR_ROOT_ID:
                printf("IFLA_BR_ROOT_ID ");
                break;
            case IFLA_BR_BRIDGE_ID:
                printf("IFLA_BR_BRIDGE_ID ");
                break;
            case IFLA_BR_ROOT_PORT:
                printf("IFLA_BR_ROOT_PORT ");
                break;
            case IFLA_BR_ROOT_PATH_COST:
                printf("IFLA_BR_ROOT_PATH_COST ");
                break;
            case IFLA_BR_TOPOLOGY_CHANGE:
                printf("IFLA_BR_TOPOLOGY_CHANGE ");
                break;
            case IFLA_BR_TOPOLOGY_CHANGE_DETECTED:
                printf("IFLA_BR_TOPOLOGY_CHANGE_DETECTED ");
                break;
            case IFLA_BR_HELLO_TIMER:
                printf("IFLA_BR_HELLO_TIMER ");
                break;
            case IFLA_BR_TCN_TIMER:
                printf("IFLA_BR_TCN_TIMER ");
                break;
            case IFLA_BR_TOPOLOGY_CHANGE_TIMER:
                printf("IFLA_BR_TOPOLOGY_CHANGE_TIMER ");
                break;
            case IFLA_BR_GC_TIMER:
                printf("IFLA_BR_GC_TIMER ");
                break;
            case IFLA_BR_GROUP_ADDR:
                printf("IFLA_BR_GROUP_ADDR ");
                break;
            case IFLA_BR_FDB_FLUSH:
                printf("IFLA_BR_FDB_FLUSH ");
                break;
            case IFLA_BR_MCAST_ROUTER:
                printf("IFLA_BR_MCAST_ROUTER ");
                break;
            case IFLA_BR_MCAST_SNOOPING:
                printf("IFLA_BR_MCAST_SNOOPING ");
                break;
            case IFLA_BR_MCAST_QUERY_USE_IFADDR:
                printf("IFLA_BR_MCAST_QUERY_USE_IFADDR ");
                break;
            case IFLA_BR_MCAST_QUERIER:
                printf("IFLA_BR_MCAST_QUERIER ");
                break;
            case IFLA_BR_MCAST_HASH_ELASTICITY:
                printf("IFLA_BR_MCAST_HASH_ELASTICITY ");
            case IFLA_BR_MCAST_HASH_MAX:
                printf("IFLA_BR_MCAST_HASH_MAX ");
                break;
            case IFLA_BR_MCAST_LAST_MEMBER_CNT:
                printf("IFLA_BR_MCAST_LAST_MEMBER_CNT ");
                break;
            case IFLA_BR_MCAST_STARTUP_QUERY_CNT:
                printf("IFLA_BR_MCAST_STARTUP_QUERY_CNT ");
                break;
            case IFLA_BR_MCAST_LAST_MEMBER_INTVL:
                printf("IFLA_BR_MCAST_LAST_MEMBER_INTVL ");
                break;
            case IFLA_BR_MCAST_MEMBERSHIP_INTVL:
                printf("IFLA_BR_MCAST_MEMBERSHIP_INTVL ");
                break;
            case IFLA_BR_MCAST_QUERIER_INTVL:
                printf("IFLA_BR_MCAST_QUERIER_INTVL ");
                break;
            case IFLA_BR_MCAST_QUERY_INTVL:
                printf("IFLA_BR_MCAST_QUERY_INTVL ");
                break;
            case IFLA_BR_MCAST_QUERY_RESPONSE_INTVL:
                printf("IFLA_BR_MCAST_QUERY_RESPONSE_INTVL ");
                break;
            case IFLA_BR_MCAST_STARTUP_QUERY_INTVL:
                printf("IFLA_BR_MCAST_STARTUP_QUERY_INTVL ");
                break;
            case IFLA_BR_NF_CALL_IPTABLES:
                printf("IFLA_BR_NF_CALL_IPTABLES ");
                break;
            case IFLA_BR_NF_CALL_IP6TABLES:
                printf("IFLA_BR_NF_CALL_IP6TABLES ");
                break;
            case IFLA_BR_NF_CALL_ARPTABLES:
                printf("IFLA_BR_NF_CALL_ARPTABLES ");
                break;
            case IFLA_BR_VLAN_DEFAULT_PVID:
                printf("IFLA_BR_VLAN_DEFAULT_PVID ");
                break;
            case IFLA_BR_PAD:
                printf("IFLA_BR_PAD ");
                break;
            case IFLA_BR_VLAN_STATS_ENABLED:
                printf("IFLA_BR_VLAN_STATS_ENABLED ");
                break;
            case IFLA_BR_MCAST_STATS_ENABLED:
                printf("IFLA_BR_MCAST_STATS_ENABLED ");
                break;
            default:
                printf("unknown bridge info type %d \n", rta->rta_type);
        }
        rta = RTA_NEXT(rta, len);
    }
}
static void nl_parse_bond_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_ipoib_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_ip6tnl_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_ipip_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_sit_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_vxlan_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_gre_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_gretap_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_ip6gre_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_ip6gretap_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_vti_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_nlmon_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_bond_slave_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_genev_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_bridge_slave_info(struct rtattr *rta, unsigned int len)
{

}
static void nl_parse_macsec_info(struct rtattr *rta, unsigned int len)
{

}

static void nl_parse_linkinfo(void *info, unsigned int len)
{
    struct rtattr *rta = NULL;
    char name[32];

    rta = (struct rtattr *)info;
    if (rta->rta_type == IFLA_INFO_KIND) {
        snprintf(name, RTA_PAYLOAD(rta) + 1, "%s", RTA_DATA(rta));
        printf("type %s\n", name);
    }
    rta = RTA_NEXT(rta, len);

    while (RTA_OK(rta, len)) {
        switch (rta->rta_type) {
            case IFLA_INFO_KIND:
                break;
            case IFLA_INFO_DATA:
                printf("link info data ");
                if (strcmp(name, "vlan") == 0) {
                    nl_parse_vlan_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "veth") == 0) {
                    nl_parse_veth_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "vcan") == 0) {
                    nl_parse_vcan_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "dummy") == 0) {
                    nl_parse_dummy_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "ifb") == 0) {
                    nl_parse_ifb_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "macvlan") == 0) {
                    nl_parse_macvlan_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "macvtap") == 0) {
                    nl_parse_macvtap_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "bridge") == 0) {
                    nl_parse_bridge_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "bond") == 0) {
                    nl_parse_bond_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "ipoib") == 0) {
                    nl_parse_ipoib_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "ip6tnl") == 0) {
                    nl_parse_ip6tnl_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "ipip") == 0) {
                    nl_parse_ipip_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "sit") == 0) {
                    nl_parse_sit_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "vxlan") == 0) {
                    nl_parse_vxlan_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "gre") == 0) {
                    nl_parse_gre_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "gretap") == 0) {
                    nl_parse_gretap_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "ip6gre") == 0) {
                    nl_parse_ip6gre_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "ip6gretap") == 0) {
                    nl_parse_ip6gretap_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "vti") == 0) {
                    nl_parse_vti_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "nlmon") == 0) {
                    nl_parse_nlmon_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "bond_slave") == 0) {
                    nl_parse_bond_slave_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "genev") == 0) {
                    nl_parse_genev_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "bridge_slave") == 0) {
                    nl_parse_bridge_slave_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else if (strcmp(name, "macsec") == 0) {
                    nl_parse_macsec_info(RTA_DATA(rta), RTA_PAYLOAD(rta));
                } else {
                    printf("no parse info func\n");
                }
                break;
            case IFLA_INFO_XSTATS:
                printf("link info xstats ");
                break;
            case IFLA_INFO_SLAVE_KIND:
                printf("link info slave kind ");
                break;
            case IFLA_INFO_SLAVE_DATA:
                printf("link info slave data ");
                break;
            default:
                printf("unknown link info type\n");
        }
        rta = RTA_NEXT(rta, len);
    }
    return ;
}

static int nl_parse_link(struct nlmsghdr *h)
{
    struct ifinfomsg *ifmsg = NULL;
    struct rtattr *rta = NULL;
    int ifla_payload = 0;
    char name[32];

    ifla_payload = IFLA_PAYLOAD(h);
    ifmsg = (struct ifinfomsg *)NLMSG_DATA(h);
    rta = (struct rtattr *)IFLA_RTA(ifmsg);

    printf("family %u, type %u, index %u, flags %x, change %x\n",
            ifmsg->ifi_family, ifmsg->ifi_type, ifmsg->ifi_index, ifmsg->ifi_flags, ifmsg->ifi_change);
    while (RTA_OK(rta, ifla_payload)) {
        switch (rta->rta_type) {
            case IFLA_UNSPEC:
                printf("IFLA_UNSPEC ");
                break;
            case IFLA_ADDRESS:
                printf("IFLA_ADDRESS ");
                nl_print_mac(RTA_DATA(rta));
                break;
            case IFLA_BROADCAST:
                printf("IFLA_BROADCAST ");
                nl_print_mac(RTA_DATA(rta));
                break;
            case IFLA_IFNAME:
                snprintf(name, RTA_PAYLOAD(rta), "%s",RTA_DATA(rta)); 
                printf("IFLA_IFNAME %s ", name);
                break;
            case IFLA_MTU://unsigned int
                printf("IFLA_MTU %u ", *(unsigned int *)RTA_DATA(rta));
                break;
            case IFLA_LINK://link type int
                printf("IFLA_LINK %d ", *(int *)RTA_DATA(rta));
                nl_get_interface_name(*(unsigned int *)RTA_DATA(rta), name);
                printf("%s ", name);
                break;
            case IFLA_QDISC://char array
                printf("IFLA_QDISC %s ", (char*)RTA_DATA(rta));
                break;
            case IFLA_STATS://struct net_device_stats
                printf("IFLA_STATS ");
                nl_print_link_stats(RTA_DATA(rta));
                break;
            case IFLA_COST:
                printf("IFLA_COST ");
                break;
            case IFLA_PRIORITY:
                printf("IFLA_PRIORITY ");
                break;
            case IFLA_MASTER:
                printf("IFLA_MASTER ");
                break;
            case IFLA_WIRELESS:
                printf("IFLA_WIRELESS ");
                break;
            case IFLA_PROTINFO:
                printf("IFLA_PROTINFO ");
                break;
            case IFLA_TXQLEN:
                printf("IFLA_TXQLEN ");
                break;
            case IFLA_MAP:
                printf("IFLA_MAP ");
                nl_print_link_map(RTA_DATA(rta));
                break;
            case IFLA_WEIGHT:
                printf("IFLA_WEIGHT ");
                break;
            case IFLA_OPERSTATE:
                printf("IFLA_OPERSTATE ");
                break;
            case IFLA_LINKMODE:
                printf("IFLA_LINKMODE %d %u ", RTA_PAYLOAD(rta), *(unsigned char *)RTA_DATA(rta));
                break;
            case IFLA_LINKINFO:
                printf("IFLA_LINKINFO ");
                nl_parse_linkinfo(RTA_DATA(rta), RTA_PAYLOAD(rta));
                break;
            case IFLA_NET_NS_PID:
                printf("IFLA_NET_NS_PID ");
                break;
            case IFLA_IFALIAS:
                printf("IFLA_IFALIAS ");
                break;
            case IFLA_NUM_VF:
                printf("IFLA_NUM_VF ");
                break;
            case IFLA_VFINFO_LIST:
                printf("IFLA_VFINFO_LIST ");
                break;
            case IFLA_STATS64:
                printf("IFLA_STATS64 ");
                break;
            case IFLA_VF_PORTS:
                printf("IFLA_VF_PORTS ");
                break;
            case IFLA_PORT_SELF:
                printf("IFLA_PORT_SELF ");
                break;
            case IFLA_AF_SPEC:
                printf("IFLA_AF_SPEC ");
                break;
            case IFLA_GROUP:
                printf("IFLA_GROUP ");
                break;
            case IFLA_NET_NS_FD:
                printf("IFLA_NET_NS_FD %u ", *(unsigned int *)RTA_DATA(rta));
                break;
            case IFLA_PROMISCUITY:
                printf("IFLA_PROMISCUITY ");
                break;
            case IFLA_NUM_TX_QUEUES:
                printf("IFLA_NUM_TX_QUEUES ");
                break;
            case IFLA_NUM_RX_QUEUES:
                printf("IFLA_NUM_RX_QUEUES ");
                break;
            case IFLA_CARRIER:
                printf("IFLA_CARRIER ");
                break;
            case IFLA_PHYS_PORT_ID:
                printf("IFLA_PHYS_PORT_ID ");
                break;
            case IFLA_CARRIER_CHANGES:
                printf("IFLA_CARRIER_CHANGES ");
                break;
            case IFLA_PHYS_SWITCH_ID:
                printf("IFLA_PHYS_SWITCH_ID ");
                break;
            case IFLA_LINK_NETNSID:
                printf("IFLA_LINK_NETNSID %u ", *(unsigned int *)RTA_DATA(rta));
                break;
            case IFLA_PHYS_PORT_NAME:
                snprintf(name, RTA_PAYLOAD(rta), "%s",RTA_DATA(rta)); 
                printf("IFLA_PHYS_PORT_NAME %s ", name);
                break;
            case IFLA_PROTO_DOWN:
                printf("IFLA_PROTO_DOWN ");
                break;
            case IFLA_GSO_MAX_SEGS:
                printf("IFLA_GSO_MAX_SEGS ");
                break;
            case IFLA_GSO_MAX_SIZE:
                printf("IFLA_GSO_MAX_SIZE ");
                break;
            case IFLA_PAD:
                printf("IFLA_PAD ");
                break;
            default:
                break;
        }
        printf("\n");
        rta = RTA_NEXT(rta, ifla_payload);
    }
    return 0;
}

void nl_print_addr(void *addr)
{
    unsigned char *p = (unsigned char *)addr;

    printf("%u.%u.%u.%u ", p[0], p[1], p[2], p[3]);
}

static void nl_print_addr_cacheinfo(struct ifa_cacheinfo *info)
{
    printf("prefered %u ", info->ifa_prefered);
    printf("valid %u ", info->ifa_valid);
    printf("cstamp %u ", info->cstamp);
    printf("tstamp %u ", info->tstamp);
}

static int nl_parse_addr(struct nlmsghdr *h)
{
    struct ifaddrmsg *ifamsg = NULL;
    struct rtattr *rta = NULL;
    int ifa_payload = 0;
    char name[32];

    ifa_payload = IFA_PAYLOAD(h);
    ifamsg = (struct ifaddrmsg *)NLMSG_DATA(h);
    rta = (struct rtattr *)IFA_RTA(ifamsg);

    printf("family %u, prefixlen %u, flags %x, scope %u, index %u\n",
            ifamsg->ifa_family, ifamsg->ifa_prefixlen, ifamsg->ifa_flags, ifamsg->ifa_scope, ifamsg->ifa_index);
    while (RTA_OK(rta, ifa_payload)) {
        switch (rta->rta_type) {
            case IFA_UNSPEC:
                printf("IFA_UNSPEC ");
                break;
            case IFA_ADDRESS:
                printf("IFA_ADDRESS %u ", RTA_PAYLOAD(rta));
                nl_print_addr(RTA_DATA(rta));
                break;
            case IFA_LOCAL:
                printf("IFA_LOCAL %u ", RTA_PAYLOAD(rta));
                nl_print_addr(RTA_DATA(rta));
                break;
            case IFA_LABEL:
                printf("IFA_LABEL %s ", RTA_DATA(rta));
                break;
            case IFA_BROADCAST:
                printf("IFA_BROADCAST %u ", RTA_PAYLOAD(rta));
                nl_print_addr(RTA_DATA(rta));
                break;
            case IFA_ANYCAST:
                printf("IFA_ANYCAST %u ", RTA_PAYLOAD(rta));
                break;
            case IFA_CACHEINFO:
                printf("IFA_CACHEINFO %u ", RTA_PAYLOAD(rta));
                nl_print_addr_cacheinfo(RTA_DATA(rta));
                break;
            case IFA_MULTICAST:
                printf("IFA_MULTICAST %u ", RTA_PAYLOAD(rta));
                break;
            case IFA_FLAGS:
                printf("IFA_FLAGS %u ", RTA_PAYLOAD(rta));
                break;
            default:
                printf("default %u ", RTA_PAYLOAD(rta));
                break;
        }
        printf("\n");
        rta = RTA_NEXT(rta, ifa_payload);
    }
    return 0;
}

static void nl_print_route_type(unsigned char rtm_type)
{
    printf("route type : ");
    switch (rtm_type) {
        case RTN_UNSPEC:
            printf("unspec\n");
            break;
        case RTN_UNICAST:
            printf("unicast\n");
            break;
        case RTN_LOCAL:
            printf("local\n");
            break;
        case RTN_BROADCAST:
            printf("broadcast\n");
            break;
        case RTN_ANYCAST:
            printf("anycast\n");
            break;
        case RTN_MULTICAST:
            printf("multicast\n");
            break;
        case RTN_BLACKHOLE:
            printf("blackhole\n");
            break;
        case RTN_UNREACHABLE:
            printf("unreachable\n");
            break;
        case RTN_PROHIBIT:
            printf("prohibit\n");
            break;
        case RTN_THROW:
            printf("throw\n");
            break;
        case RTN_NAT:
            printf("nat\n");
            break;
        case RTN_XRESOLVE:
            printf("xresolve\n");
            break;
        default:
            printf("unknown route type\n");
            break;
    }
}

static void nl_print_route_protocol(unsigned char rtm_protocol)
{
    printf("route protocol : ");
    switch (rtm_protocol) {
        case RTPROT_UNSPEC:
            printf("unspec\n");
            break;
        case RTPROT_REDIRECT:
            printf("redirect\n");
            break;
        case RTPROT_KERNEL:
            printf("kernel\n");
            break;
        case RTPROT_BOOT:
            printf("boot\n");
            break;
        case RTPROT_STATIC:
            printf("static\n");
            break;
        default:
            printf("unknown route protocol %u \n", (unsigned int)rtm_protocol);
    }
}

static int nl_print_route_scope(unsigned char rtm_scope)
{
    printf("route scope : ");
    switch (rtm_scope) {
        case RT_SCOPE_UNIVERSE:
            printf("global route\n");
            break;
        case RT_SCOPE_SITE:
            printf("interior route\n");
            break;
        case RT_SCOPE_LINK:
            printf("route on this link\n");
            break;
        case RT_SCOPE_HOST:
            printf("route on this host\n");
            break;
        case RT_SCOPE_NOWHERE:
            printf("destination doesn't exist\n");
            break;
        default:
            printf("unknown route scope %u \n", (unsigned int)rtm_scope);
    }
}

static void nl_print_route_flags(unsigned int rtm_flags)
{
    printf("route flags : ");
    if (rtm_flags & RTM_F_NOTIFY) {
        printf("notify ");
    }
    if (rtm_flags & RTM_F_CLONED) {
        printf("cloned ");
    }
    if (rtm_flags & RTM_F_EQUALIZE) {
        printf("equalize ");
    }
    if (rtm_flags == 0) {
        printf("no flags");
    }

    printf("\n");
}

static void nl_print_route_table(unsigned char rtm_table)
{
    printf("route table : ");
    switch (rtm_table) {
        case RT_TABLE_UNSPEC:
            printf("unspec\n");
            break;
        case RT_TABLE_COMPAT:
            printf("compat\n");
            break;
        case RT_TABLE_DEFAULT:
            printf("default\n");
            break;
        case RT_TABLE_MAIN:
            printf("main\n");
            break;
        case RT_TABLE_LOCAL:
            printf("local\n");
            break;
        default:
            printf("unknown route table %u\n", (unsigned int)rtm_table);
            break;
    }
}

static int nl_parse_route(struct nlmsghdr *h)
{
    struct rtmsg *rtmsg = NULL;
    struct rtattr *rta = NULL;
    int rt_payload = 0;
    char name[32];

    rt_payload = NLMSG_PAYLOAD(h, sizeof(struct rtmsg));
    rtmsg = (struct rtmsg *)NLMSG_DATA(h);
    rta = (struct rtattr *)RTM_RTA(rtmsg);

    printf("family %u, dst len %u, src len %u, tos %u, table %u, protocol %u, scope %u, type %u, flags %u\n",
            rtmsg->rtm_family, rtmsg->rtm_dst_len, rtmsg->rtm_src_len, rtmsg->rtm_tos, rtmsg->rtm_table, rtmsg->rtm_protocol, rtmsg->rtm_scope, rtmsg->rtm_type, rtmsg->rtm_flags);

    nl_print_route_type(rtmsg->rtm_type);
    nl_print_route_protocol(rtmsg->rtm_protocol);
    nl_print_route_scope(rtmsg->rtm_scope);
    nl_print_route_flags(rtmsg->rtm_flags);
    nl_print_route_table(rtmsg->rtm_table);

    while (RTA_OK(rta, rt_payload)) {
        switch (rta->rta_type) {
            case RTA_UNSPEC:
                printf("RTA_UNSPEC %u ", RTA_PAYLOAD(rta)); 
                break;
            case RTA_DST:
                printf("RTA_DST %u ", RTA_PAYLOAD(rta));
                nl_print_addr(RTA_DATA(rta));
                break;
            case RTA_SRC:
                printf("RTA_SRC %u ", RTA_PAYLOAD(rta));
                nl_print_addr(RTA_DATA(rta));
                break;
            case RTA_IIF:
                printf("RTA_IIF %u ", RTA_PAYLOAD(rta));
                printf("interface index %d ", *(int *)RTA_DATA(rta));
                break;
            case RTA_OIF:
                printf("RTA_OIF %u ", RTA_PAYLOAD(rta));
                printf("interface index %d ", *(int *)RTA_DATA(rta));
                break;
            case RTA_GATEWAY:
                printf("RTA_GATEWAY %u ", RTA_PAYLOAD(rta));
                nl_print_addr(RTA_DATA(rta));
                break;
            case RTA_PRIORITY:
                printf("RTA_PRIORITY %u ", RTA_PAYLOAD(rta));
                printf("priority %d ", *(int *)RTA_DATA(rta));
                break;
            case RTA_PREFSRC:
                printf("RTA_PREFSRC %u ", RTA_PAYLOAD(rta));
                nl_print_addr(RTA_DATA(rta));
                break;
            case RTA_METRICS:
                printf("RTA_METRICS %u ", RTA_PAYLOAD(rta));
                printf("metric %d ", *(int *)RTA_DATA(rta));
                break;
            case RTA_MULTIPATH:
                printf("RTA_MULTIPATH %u ", RTA_PAYLOAD(rta));
                break;
            case RTA_PROTOINFO:
                printf("RTA_PROTOINFO %u ", RTA_PAYLOAD(rta));
                break;
            case RTA_FLOW:
                printf("RTA_FLOW %u ", RTA_PAYLOAD(rta));
                break;
            case RTA_CACHEINFO:
                printf("RTA_CACHEINFO %u ", RTA_PAYLOAD(rta));
                break;
            case RTA_SESSION:
                printf("RTA_SESSION %u ", RTA_PAYLOAD(rta));
                break;
            case RTA_MP_ALGO:
                printf("RTA_MP_ALGO %u ", RTA_PAYLOAD(rta));
                break;
            case RTA_TABLE:
                printf("RTA_TABLE %u %u ", RTA_PAYLOAD(rta), *(unsigned int *)RTA_DATA(rta));
                break;
            case RTA_MARK:
                printf("RTA_MARK %u ", RTA_PAYLOAD(rta));
                break;
            case RTA_MFC_STATS:
                printf("RTA_MFC_STATS %u ", RTA_PAYLOAD(rta));
                break;
            case __RH_RESERVED_RTA_VIA:
                printf("__RH_RESERVED_RTA_VIA %u ", RTA_PAYLOAD(rta));
                break;
            case __RH_RESERVED_RTA_NEWDST:
                printf("__RH_RESERVED_RTA_NEWDST %u ", RTA_PAYLOAD(rta));
                break;
            case __RH_RESERVED_RTA_PREF:
                printf("__RH_RESERVED_RTA_PREF %u ", RTA_PAYLOAD(rta));
                break;
            case RTA_ENCAP_TYPE:
                printf("RTA_ENCAP_TYPE %u ", RTA_PAYLOAD(rta));
                break;
            case RTA_ENCAP:
                printf("RTA_ENCAP %u ", RTA_PAYLOAD(rta));
                break;
            case RTA_EXPIRES:
                printf("RTA_EXPIRES %u ", RTA_PAYLOAD(rta));
                break;
            case RTA_PAD:
                printf("RTA_PAD %u ", RTA_PAYLOAD(rta));
                break;
            default:
                printf("default %u ", RTA_PAYLOAD(rta));
                break;
        }
        printf("\n");
        rta = RTA_NEXT(rta, rt_payload);
    }
    return 0;
}

static int nl_parse_neighbor(struct nlmsghdr *h)
{
    int ret;
    struct ndmsg *ndmsg = NULL;
    struct rtattr *rta = NULL;
    int rta_payload = 0;

    rta_payload = NLMSG_PAYLOAD(h, sizeof(struct ndmsg));
    ndmsg = (struct ndmsg *)NLMSG_DATA(h);
    rta = (struct rtattr *)((char *)ndmsg + NLMSG_ALIGN(sizeof(struct ndmsg)));

    while (RTA_OK(rta, rta_payload)) {
        switch(rta->rta_type) {
            case NDA_UNSPEC:
                printf("NDA_UNSPEC ");
                break;
            case NDA_DST:
                printf("NDA_DST ");
                break;
            case NDA_LLADDR:
                printf("NDA_LLADDR ");
                break;
            case NDA_CACHEINFO:
                printf("NDA_CACHEINFO ");
                break;
            case NDA_PROBES:
                printf("NDA_PROBES ");
                break;
            case NDA_VLAN:
                printf("NDA_VLAN ");
                break;
            case NDA_PORT:
                printf("NDA_PORT ");
                break;
            case NDA_VNI:
                printf("NDA_VNI ");
                break;
            case NDA_IFINDEX:
                printf("NDA_IFINDEX ");
                break;
            case NDA_MASTER:
                printf("NDA_MASTER ");
                break;
            case NDA_LINK_NETNSID:
                printf("NDA_LINK_NETNSID ");
                break;
            default:
                printf("default ");
                break;
        }
        rta = RTA_NEXT(rta, rta_payload);
    }
    return 0;
}

static int nl_parse_traffic_control(struct nlmsghdr *h)
{
    struct tcmsg *tcmsg = NULL;
    struct rtattr *rta = NULL;
    int rta_payload = 0;

    rta_payload = TCA_PAYLOAD(h);
    tcmsg = (struct tcmsg *)NLMSG_DATA(h);
    rta = TCA_RTA(tcmsg);

    while (RTA_OK(rta, rta_payload)) {
        switch(rta->rta_type) {
            case TCA_UNSPEC:
                printf("TCA_UNSPEC ");
                break;
            case TCA_KIND:
                printf("TCA_KIND ");
                break;
            case TCA_OPTIONS:
                printf("TCA_OPTIONS ");
                break;
            case TCA_STATS:
                printf("TCA_STATS ");
                break;
            case TCA_XSTATS:
                printf("TCA_XSTATS ");
                break;
            case TCA_RATE:
                printf("TCA_RATE ");
                break;
            case TCA_FCNT:
                printf("TCA_FCNT ");
                break;
            case TCA_STATS2:
                printf("TCA_STATS2 ");
                break;
            case TCA_STAB:
                printf("TCA_STAB ");
                break;
            case TCA_PAD:
                printf("TCA_PAD ");
                break;
            default:
                printf("default ");
                break;
        }
        rta = RTA_NEXT(rta, rta_payload);
    }
    return 0;
}

static int nl_parse_qdisc(struct nlmsghdr *h)
{
    return nl_parse_traffic_control(h);
}

static int nl_parse_tclass(struct nlmsghdr *h)
{
    return nl_parse_traffic_control(h);
}

static int nl_parse_tfilter(struct nlmsghdr *h)
{
    return nl_parse_traffic_control(h);
}

int nl_parse_netlink_message(char *buff, int len)
{
    int ret;
    struct nlmsghdr *h;
    struct nlmsgerr *he = NULL;

    h = (struct nlmsghdr *)buff;

    while (NLMSG_OK(h, len)) {
        switch (h->nlmsg_type) {
            case NLMSG_DONE:
                printf("NLMSG_DONE ");
                break;
            case NLMSG_ERROR:
                he = NLMSG_DATA(h);
                printf("NLMSG_ERROR %d %s ", he->error, strerror(-(he->error)));

                printf("NLMSG_ERROR ");
                break;
            case RTM_NEWLINK://struct ifinfomsg
            case RTM_DELLINK:
            case RTM_GETLINK:
            case RTM_SETLINK:
                printf("RTM_LINK");
                nl_parse_link(h);
                break;
            case RTM_NEWADDR://struct ifaddrmsg
            case RTM_DELADDR:
            case RTM_GETADDR:
                printf("RTM_ADDR ");
                nl_parse_addr(h);
                break;
            case RTM_NEWROUTE://struct rtmsg
            case RTM_DELROUTE:
            case RTM_GETROUTE:
                printf("RTM_ROUTE ");
                nl_parse_route(h);
                break;
            case RTM_NEWNEIGH://struct ndmsg
            case RTM_DELNEIGH:
            case RTM_GETNEIGH:
                printf("RTM_NEIGH ");
                nl_parse_neighbor(h);
                break;
            case RTM_NEWRULE://struct fib_rule_hdr
            case RTM_DELRULE:
            case RTM_GETRULE:
                printf("RTM_RULE ");
                nl_parse_route(h);
                break;
            case RTM_NEWQDISC://struct tcmsg
            case RTM_DELQDISC:
            case RTM_GETQDISC:
                printf("RTM_QDISC ");
                nl_parse_qdisc(h);
                break;
            case RTM_NEWTFILTER://struct tcmsg
            case RTM_DELTFILTER:
            case RTM_GETTFILTER:
                printf("RTM_TFILTER ");
                nl_parse_tfilter(h);
                break;
            case RTM_NEWTCLASS://struct tcmsg
            case RTM_DELTCLASS:
            case RTM_GETTCLASS:
                printf("RTM_CLASS ");
                nl_parse_tclass(h);
                break;
            case RTM_NEWACTION://struct tcamsg
            case RTM_DELACTION:
            case RTM_GETACTION:
                printf("RTM_ACTION ");
                break;
            case RTM_NEWPREFIX: //struct prefixmsg
                break;
            case RTM_GETMULTICAST:
                break;
            case RTM_GETANYCAST:
                break;
            case RTM_NEWNEIGHTBL://struct ndtmsg
            case RTM_GETNEIGHTBL:
            case RTM_SETNEIGHTBL:
                printf("RTM_NEIGHTBL ");
                break;
            case RTM_NEWNDUSEROPT://struct nduseroptmsg
                break;
            case RTM_NEWADDRLABEL://struct ifaddrlblmsg
            case RTM_DELADDRLABEL:
            case RTM_GETADDRLABEL:
                break;
            case RTM_GETDCB:
                break;
            case RTM_SETDCB:
                break;
            case RTM_NEWNETCONF:
                break;
            case RTM_GETNETCONF:
                break;
            case RTM_NEWMDB://struct br_port_msg
            case RTM_DELMDB:
            case RTM_GETMDB:
                break;
            case RTM_NEWNSID://struct rtgenmsg
            case RTM_DELNSID:
            case RTM_GETNSID:
                printf("RTM_NS ");
                break;
            case RTM_NEWSTATS://struct if_stats_msg
            case RTM_GETSTATS:
                break;
            default:
                printf("LINK TYPE %d", h->nlmsg_type);
                break;
        }
        printf("\n");
        h = NLMSG_NEXT(h, len);
    }
    return 0;
}

/*

int nl_list_neighbor(int fd, void *buff, int buff_len)
{
    int ret = 0;
    struct {
        struct nlmsghdr h;
        struct ndmsg n;
    } req = {
        .h.nlmsg_len = NLMSG_LENGTH(sizeof(struct ndmsg)),
        .h.nlmsg_type = RTM_GETNEIGH,
        .h.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP,
        .n.ndm_family = AF_INET,
    };
    
    ret = nl_send(fd, &req, req.h.nlmsg_len);
    if (ret < 0) {
        return ret;
    }

    return nl_recv(fd, buff, buff_len);
}

static int nl_list_traffic_control(int fd, void *buff, int buff_len, unsigned short type)
{
    int ret = 0;
    struct {
        struct nlmsghdr h;
        struct tcmsg t;
    } req = {
        .h.nlmsg_len = NLMSG_LENGTH(sizeof(struct tcmsg)),
        .h.nlmsg_type = type,
        .h.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP,
        .t.tcm_family = AF_INET,
    };

    ret = nl_send(fd, &req, req.h.nlmsg_len);
    if (ret < 0) {
        return ret;
    }

    return nl_recv(fd, buff, buff_len);
}

int nl_list_qdisc(int fd, void *buff, int buff_len)
{
    return nl_list_traffic_control(fd, buff, buff_len, RTM_GETQDISC);
}

int nl_list_tclass(int fd, void *buff, int buff_len)
{
    return nl_list_traffic_control(fd, buff, buff_len, RTM_GETTCLASS);
}

int nl_list_tfilter(int fd, void *buff, int buff_len)
{
    return nl_list_traffic_control(fd, buff, buff_len, RTM_GETTFILTER);
}
*/
