#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kprobes.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bo Chen (chenbo@pdx.edu)");
MODULE_DESCRIPTION("CRETE probes for kernel API functions to inject concolic values");

//#define CRETE_ENABLE_DEBUG

#ifdef CRETE_ENABLE_DEBUG
#define CRETE_DBG(x) do { x } while(0)
#else
#define CRETE_DBG(x) do { } while(0)
#endif

#if defined(CRETE_USED_OLD_MODULE_LAYOUT)
#define __USED_OLD_MODULE_LAYOUT
#endif
static char crete_ksym_symbol[KSYM_SYMBOL_LEN*2];

struct TargetModuleInfo
{
    size_t m_name_size;
    char *m_name;
    int   m_mod_loaded;
    struct module m_mod;
};

static struct TargetModuleInfo target_module = {
        .m_name_size = 0,
        .m_name = "",
        .m_mod_loaded = 0,
};

module_param_named(target_module, target_module.m_name, charp, 0);
MODULE_PARM_DESC(target_module, "The name of target module to enable probe on kernel APIs");

static void (*_crete_make_concolic)(void*, size_t, const char *);
static void (*_crete_kernel_oops)(void);

static bool target_module_probes = false;
static void (*_crete_register_probes_target_module)(void);
static void (*_crete_unregister_probes_target_module)(void);

static int entry_handler_default(struct kretprobe_instance *ri, struct pt_regs *regs);
static int ret_handler_make_concolic(struct kretprobe_instance *ri, struct pt_regs *regs);

#define __CRETE_DEF_KPROBE(func_name)                                                              \
        static int entry_handler_##func_name(struct kretprobe_instance *ri, struct pt_regs *regs); \
        static int ret_handler_##func_name(struct kretprobe_instance *ri, struct pt_regs *regs);   \
        static struct kretprobe kretp_##func_name = {                                              \
                .kp.symbol_name = #func_name,                                                      \
                .entry_handler = entry_handler_##func_name,                                        \
                .handler = ret_handler_##func_name,                                                \
        };

#define __CRETE_DEF_KPROBE_RET_CONCOLIC(func_name)                                                 \
        static struct kretprobe kretp_##func_name = {                                              \
                .kp.symbol_name = #func_name,                                                      \
                .handler = ret_handler_make_concolic,                                              \
                .entry_handler = entry_handler_default,                                            \
        };

#define __CRETE_REG_KPROBE(func_name)                                          \
        if(kallsyms_lookup_name(#func_name))                                   \
        {                                                                      \
            if(register_kretprobe(&kretp_##func_name))                         \
            {                                                                  \
                printk(KERN_INFO "kprobe register failed for "#func_name"\n"); \
                return -1;                                                     \
            }                                                                  \
        }

#define __CRETE_UNREG_KPROBE(func_name)                 \
        if(kallsyms_lookup_name(#func_name))            \
        {                                               \
            unregister_kretprobe(&kretp_##func_name);   \
        }


/* ------------------------------- */
// Define interested functions to hook
__CRETE_DEF_KPROBE(oops_enter);

// alloc related
__CRETE_DEF_KPROBE_RET_CONCOLIC(__kmalloc) // kmalloc, kcalloc
__CRETE_DEF_KPROBE_RET_CONCOLIC(__vmalloc)
__CRETE_DEF_KPROBE_RET_CONCOLIC(vzalloc)
__CRETE_DEF_KPROBE_RET_CONCOLIC(__alloc_skb) // alloc_skb
__CRETE_DEF_KPROBE_RET_CONCOLIC(__napi_alloc_skb)
//__CRETE_DEF_KPROBE_RET_CONCOLIC(__alloc_pages_nodemask) // invoked from a loop in e1000
__CRETE_DEF_KPROBE_RET_CONCOLIC(netdev_alloc_frag)
__CRETE_DEF_KPROBE_RET_CONCOLIC(alloc_etherdev_mqs)
__CRETE_DEF_KPROBE_RET_CONCOLIC(__alloc_ei_netdev)
__CRETE_DEF_KPROBE_RET_CONCOLIC(alloc_pages_current)
__CRETE_DEF_KPROBE_RET_CONCOLIC(arch_dma_alloc_attrs)
__CRETE_DEF_KPROBE_RET_CONCOLIC(dma_pool_alloc)
__CRETE_DEF_KPROBE_RET_CONCOLIC(kmem_cache_alloc_trace)
__CRETE_DEF_KPROBE_RET_CONCOLIC(__netdev_alloc_skb)
__CRETE_DEF_KPROBE_RET_CONCOLIC(scsi_host_alloc)
__CRETE_DEF_KPROBE_RET_CONCOLIC(snd_dma_alloc_pages)
__CRETE_DEF_KPROBE_RET_CONCOLIC(snd_hdac_bus_alloc_stream_pages)
__CRETE_DEF_KPROBE_RET_CONCOLIC(snd_pcm_lib_malloc_pages)
__CRETE_DEF_KPROBE_RET_CONCOLIC(snd_pcm_lib_preallocate_pages_for_all)

// PCI: ptr ret
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_get_device)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_get_domain_bus_and_slot)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_iomap)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_ioremap_bar)
__CRETE_DEF_KPROBE_RET_CONCOLIC(snd_pci_quirk_lookup)
// PCI: int ret
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_choose_state)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_enable_device)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_enable_device_mem)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_enable_msi_range)
__CRETE_DEF_KPROBE_RET_CONCOLIC(__pci_enable_wake)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_prepare_to_sleep)
__CRETE_DEF_KPROBE_RET_CONCOLIC(__pci_register_driver)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_request_region)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_request_regions)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_request_selected_regions)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_save_state)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_select_bars)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_set_mwi)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_set_power_state)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pci_wake_from_d3)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pcix_get_mmrbc)
__CRETE_DEF_KPROBE_RET_CONCOLIC(pcix_set_mmrbc)

// DMA:
__CRETE_DEF_KPROBE_RET_CONCOLIC(dma_pool_create)
__CRETE_DEF_KPROBE_RET_CONCOLIC(dma_supported)

//__CRETE_DEF_KPROBE_RET_CONCOLIC()

static inline int register_probes(void)
{
    __CRETE_REG_KPROBE(oops_enter);

    // alloc related
    __CRETE_REG_KPROBE(__kmalloc);
    __CRETE_REG_KPROBE(__vmalloc);
    __CRETE_REG_KPROBE(vzalloc);
    __CRETE_REG_KPROBE(__alloc_skb);
    __CRETE_REG_KPROBE( __napi_alloc_skb);
    __CRETE_REG_KPROBE(netdev_alloc_frag);
    __CRETE_REG_KPROBE(alloc_etherdev_mqs);
    __CRETE_REG_KPROBE(__alloc_ei_netdev);
    __CRETE_REG_KPROBE(alloc_pages_current);
    __CRETE_REG_KPROBE(arch_dma_alloc_attrs);
    __CRETE_REG_KPROBE(dma_pool_alloc);
    __CRETE_REG_KPROBE(kmem_cache_alloc_trace);
    __CRETE_REG_KPROBE(__netdev_alloc_skb);
    __CRETE_REG_KPROBE(scsi_host_alloc);
    __CRETE_REG_KPROBE(snd_dma_alloc_pages);
    __CRETE_REG_KPROBE(snd_hdac_bus_alloc_stream_pages);
    __CRETE_REG_KPROBE(snd_pcm_lib_malloc_pages);
    __CRETE_REG_KPROBE(snd_pcm_lib_preallocate_pages_for_all);

    // PCI: ptr ret
    __CRETE_REG_KPROBE(pci_get_device)
    __CRETE_REG_KPROBE(pci_get_domain_bus_and_slot)
    __CRETE_REG_KPROBE(pci_iomap)
    __CRETE_REG_KPROBE(pci_ioremap_bar)
    __CRETE_REG_KPROBE(snd_pci_quirk_lookup)

    // PCI: int return
    __CRETE_REG_KPROBE(pci_choose_state)
    __CRETE_REG_KPROBE(pci_enable_device)
    __CRETE_REG_KPROBE(pci_enable_device_mem)
    __CRETE_REG_KPROBE(pci_enable_msi_range)
    __CRETE_REG_KPROBE(__pci_enable_wake)
    __CRETE_REG_KPROBE(pci_prepare_to_sleep)
    __CRETE_REG_KPROBE(__pci_register_driver)
    __CRETE_REG_KPROBE(pci_request_region)
    __CRETE_REG_KPROBE(pci_request_regions)
    __CRETE_REG_KPROBE(pci_request_selected_regions)
    __CRETE_REG_KPROBE(pci_save_state)
    __CRETE_REG_KPROBE(pci_select_bars)
    __CRETE_REG_KPROBE(pci_set_mwi)
    __CRETE_REG_KPROBE(pci_set_power_state)
    __CRETE_REG_KPROBE(pci_wake_from_d3)
    __CRETE_REG_KPROBE(pcix_get_mmrbc)
    __CRETE_REG_KPROBE(pcix_set_mmrbc)

    // DMA:
    __CRETE_REG_KPROBE(dma_pool_create);
    __CRETE_REG_KPROBE(dma_supported);
//    __CRETE_REG_KPROBE();

    return 0;
}

static inline void unregister_probes(void)
{
    __CRETE_UNREG_KPROBE(oops_enter);

    __CRETE_UNREG_KPROBE(__kmalloc);
    __CRETE_UNREG_KPROBE(__vmalloc);
    __CRETE_UNREG_KPROBE(vzalloc);
    __CRETE_UNREG_KPROBE(__alloc_skb);
    __CRETE_UNREG_KPROBE( __napi_alloc_skb);
    __CRETE_UNREG_KPROBE(netdev_alloc_frag);
    __CRETE_UNREG_KPROBE(alloc_etherdev_mqs);
    __CRETE_UNREG_KPROBE(__alloc_ei_netdev);
    __CRETE_UNREG_KPROBE(alloc_pages_current);
    __CRETE_UNREG_KPROBE(arch_dma_alloc_attrs);
    __CRETE_UNREG_KPROBE(dma_pool_alloc);
    __CRETE_UNREG_KPROBE(kmem_cache_alloc_trace);
    __CRETE_UNREG_KPROBE(__netdev_alloc_skb);
    __CRETE_UNREG_KPROBE(scsi_host_alloc);
    __CRETE_UNREG_KPROBE(snd_dma_alloc_pages);
    __CRETE_UNREG_KPROBE(snd_hdac_bus_alloc_stream_pages);
    __CRETE_UNREG_KPROBE(snd_pcm_lib_malloc_pages);
    __CRETE_UNREG_KPROBE(snd_pcm_lib_preallocate_pages_for_all);

    // PCI: ptr ret
    __CRETE_UNREG_KPROBE(pci_get_device)
    __CRETE_UNREG_KPROBE(pci_get_domain_bus_and_slot)
    __CRETE_UNREG_KPROBE(pci_iomap)
    __CRETE_UNREG_KPROBE(pci_ioremap_bar)
    __CRETE_UNREG_KPROBE(snd_pci_quirk_lookup)
    // PCI: int ret
    __CRETE_UNREG_KPROBE(pci_choose_state)
    __CRETE_UNREG_KPROBE(pci_enable_device)
    __CRETE_UNREG_KPROBE(pci_enable_device_mem)
    __CRETE_UNREG_KPROBE(pci_enable_msi_range)
    __CRETE_UNREG_KPROBE(__pci_enable_wake)
    __CRETE_UNREG_KPROBE(pci_prepare_to_sleep)
    __CRETE_UNREG_KPROBE(__pci_register_driver)
    __CRETE_UNREG_KPROBE(pci_request_region)
    __CRETE_UNREG_KPROBE(pci_request_regions)
    __CRETE_UNREG_KPROBE(pci_request_selected_regions)
    __CRETE_UNREG_KPROBE(pci_save_state)
    __CRETE_UNREG_KPROBE(pci_select_bars)
    __CRETE_UNREG_KPROBE(pci_set_mwi)
    __CRETE_UNREG_KPROBE(pci_set_power_state)
    __CRETE_UNREG_KPROBE(pci_wake_from_d3)
    __CRETE_UNREG_KPROBE(pcix_get_mmrbc)
    __CRETE_UNREG_KPROBE(pcix_set_mmrbc)

    // DMA:
    __CRETE_UNREG_KPROBE(dma_pool_create);
    __CRETE_UNREG_KPROBE(dma_supported);
//    __CRETE_UNREG_KPROBE();
}

/* ------------------------------- */
// Define entry handlers for each interested function

//static int entry_handler___kmalloc(struct kretprobe_instance *ri, struct pt_regs *regs)
//{
//    char *sp_regs = (char *)kernel_stack_pointer(regs);
//    printk(KERN_INFO "entry_handler_kmalloc() entered.\n");
//    _crete_make_concolic(&regs->cx, sizeof(int), "e1000_ioctl_arg3");
//    return 0;
//}

static int entry_handler_default(struct kretprobe_instance *ri, struct pt_regs *regs)
{
    if(!current->mm)
        return 1;  // Skip kernel threads
    else
        return 0;
}

// Only inject concolic values if:
// 1. its caller is within target_module AND
// 2. the convention on return value holds
static int ret_handler_make_concolic(struct kretprobe_instance *ri, struct pt_regs *regs)
{
    // NOTE: xxx inject concolics only for module_core (not for module_init)
    if(!(target_module.m_mod_loaded &&
         (within_module_core((unsigned long)ri->ret_addr, &target_module.m_mod))))
    {
        return 0;
    }

    if(regs->ax != regs_return_value(regs))
    {
        printk(KERN_INFO  "[CRETE ERROR] Wrong assumption on return value convention about \'%s\'\n",
                ri->rp->kp.symbol_name);
        return 0;
    }

    if(!_crete_make_concolic)
    {
        CRETE_DBG(printk(KERN_INFO  "[CRETE ERROR] \'_crete_make_concolic() is not initialized\'\n"););
        return 0;
    }

    {
        unsigned long offset;

#if defined(__USED_OLD_MODULE_LAYOUT)
        offset = (unsigned long)ri->ret_addr - (unsigned long)target_module.m_mod.module_core;
#else
        offset = (unsigned long)ri->ret_addr - (unsigned long)target_module.m_mod.core_layout.base;
#endif

        // Naming convention: func_name[mod_name.module_core+offset]
        // 'offset' is normally offset from .text section of a .ko file,
        // and can be used with addr2line to find the line number in source
        // code, e.g.: 'eu-addr2line -f -e $(modinfo -n mod_name) -j .text offset'
        sprintf(crete_ksym_symbol, "%s[%s.module_core+%#lx]",
                ri->rp->kp.symbol_name, target_module.m_name, offset);

        CRETE_DBG(
        printk(KERN_INFO "ret_handler \'%s\': ret = %p (offset = %p)\n"
                "crete_ksym_symbol = %s\n",
                ri->rp->kp.symbol_name, (void *)regs->ax, (void *)offset,
                crete_ksym_symbol);
        );

        _crete_make_concolic(&regs->ax, sizeof(regs->ax), crete_ksym_symbol);
    }

    return 0;
}

static int entry_handler_oops_enter(struct kretprobe_instance *ri, struct pt_regs *regs)
{
    _crete_kernel_oops();

    return 0;
}

static int ret_handler_oops_enter(struct kretprobe_instance *ri, struct pt_regs *regs)
{
    return 0;
}

/* ------------------------------- */

static int crete_kapi_module_event(struct notifier_block *self, unsigned long event, void *data)
{
    struct module *m = data;

    if(strlen(m->name) != target_module.m_name_size)
        return NOTIFY_DONE;

    if(strncmp(m->name, target_module.m_name, target_module.m_name_size) != 0)
        return NOTIFY_DONE;

    switch (event) {
    case MODULE_STATE_COMING:
        printk(KERN_INFO "MODULE_STATE_COMING: %s\n", m->name);
        target_module.m_mod_loaded = 1;
#if defined(__USED_OLD_MODULE_LAYOUT)
        target_module.m_mod.module_core = m->module_core;
        target_module.m_mod.core_size = m->core_size;
        target_module.m_mod.module_init = m->module_init;
        target_module.m_mod.init_size = m->init_size;
#else
        target_module.m_mod.core_layout = m->core_layout;
        target_module.m_mod.init_layout = m->init_layout;
#endif
        if(target_module_probes)
            _crete_register_probes_target_module();
        break;

    case MODULE_STATE_GOING:
        printk(KERN_INFO "MODULE_STATE_GOING: %s\n", m->name);
        target_module.m_mod_loaded = 0;
        if(target_module_probes)
            _crete_unregister_probes_target_module();
        break;

    case MODULE_STATE_LIVE:
    default:
        break;
    }

    return NOTIFY_DONE;
}

static struct notifier_block crete_kapi_module_probe= {
        .notifier_call = crete_kapi_module_event,
        .priority = 1,
};

static inline int init_crete_intrinsics(void)
{
    _crete_make_concolic = (void *)kallsyms_lookup_name("crete_make_concolic");
    _crete_kernel_oops = (void *)kallsyms_lookup_name("crete_kernel_oops");

    if (!(_crete_make_concolic && _crete_kernel_oops)) {
        printk(KERN_INFO "[crete] not all function found, please check crete-intrinsics.ko\n");
        return -1;
    }

    _crete_register_probes_target_module = (void *)kallsyms_lookup_name("crete_register_probes_e1000");
    _crete_unregister_probes_target_module = (void *)kallsyms_lookup_name("crete_unregister_probes_e1000");

    if(_crete_register_probes_target_module &&
            _crete_unregister_probes_target_module)
    {
        printk(KERN_INFO "target_module_probes found!\n");
        target_module_probes = true;
    }

    return 0;
}

static int __init crete_kprobe_init(void)
{
    if(init_crete_intrinsics())
        return -1;

    if(register_probes())
        return -1;

    target_module.m_name_size = strlen(target_module.m_name);
    register_module_notifier(&crete_kapi_module_probe);

    CRETE_DBG(printk(KERN_INFO "size = %zu, name = %s\n", target_module.m_name_size, target_module.m_name););

    return 0;
}

static void __exit crete_kprobe_exit(void)
{
    unregister_module_notifier(&crete_kapi_module_probe);
    unregister_probes();
}

module_init(crete_kprobe_init)
module_exit(crete_kprobe_exit)